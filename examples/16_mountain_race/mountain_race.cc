// *************************************************************
// File:    fast_race.cc
// Descr:   simple terrain game
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <iomanip> 

#include "lib/data/cfg_loader.h"

#include "lib/audio/audio_fx.h"

#include "lib/window/gl_window.h"
#include "lib/window/helpers.h"

#include "lib/draw/gl_render_ctx.h"
#include "lib/draw/gl_object.h"
#include "lib/draw/gl_draw.h"
#include "lib/draw/gl_triangle.h"
#include "lib/draw/fx_colors.h"
#include "lib/draw/gl_lights.h"
#include "lib/draw/gl_buffer.h"
#include "lib/draw/gl_coords.h"
#include "lib/draw/gl_z_buffer.h"

#include "lib/extras/skybox.h"
#include "lib/extras/terrain.h"
#include "lib/extras/nature.h"
#include "lib/extras/water.h"
#include "lib/extras/player.h"
#include "lib/extras/cameraman.h"

#include "lib/system/timer.h"
#include "lib/system/fps_counter.h"
#include "lib/system/rand_toolkit.h"

#include "lib/math/trig.h"
#include "lib/math/matrix_rotate_eul.h"
#include "lib/math/matrix_rotate_uvn.h"
#include "lib/math/matrix_trans.h"

#include "../helpers.h"

using namespace anshub;
using namespace helpers;
using vector::operator<<;

int main(int argc, const char** argv)
{
  if (argc != 2)
  {
    std::cerr << "You must input config file name" << '\n';
    return false;
  }
  Config cfg {argv[1]};

  // Math processor
  
  TrigTable trig {};
  rand_toolkit::start_rand();

  // Timers

  FpsCounter fps {};
  Timer timer (cfg.Get<int>("win_fps"));

  // Constants

  const int kWinWidth {cfg.Get<int>("win_w")};
  const int kWinHeight {cfg.Get<int>("win_h")};
  const bool kDebugShow {cfg.Get<bool>("dbg_show_info")};  

  FColor kWhite  {255.0f, 255.0f, 255.0f};
  FColor kYellow {255.0f, 255.0f, 0.0f};
  FColor kBlue   {0.0f, 0.0f, 255.0f};
  
  // Window
  
  auto pos  = io_helpers::GetXYToMiddle(kWinWidth, kWinHeight); 
  auto mode = io_helpers::FindVideoMode(kWinWidth, kWinHeight);
  GlWindow win (pos.x, pos.y, kWinWidth, kWinHeight, "Terrain");
  
  if (cfg.Get<bool>("win_fs"))
    win.ToggleFullscreen(mode);

  // Audio

  AudioFx audio {};
  auto engine_snd = cfg.Get<std::string>("snd_engine");
  auto ambient_snd = cfg.Get<std::string>("snd_ambient");
  auto engine_mod = AudioFx::Modifier::PITCH;

  if (!engine_snd.empty())
  {
    auto min_pitch = cfg.Get<float>("snd_low_pitch");
    auto max_pitch = cfg.Get<float>("snd_high_pitch");
    audio.LoadFx(engine_snd, true);
    audio.SetModifierRange(engine_snd, engine_mod, min_pitch, max_pitch);
    audio.SetModifierValue(engine_snd, engine_mod, min_pitch);
    audio.PlayFx(engine_snd);
  }

  if (!ambient_snd.empty())
  {
    audio.Load(ambient_snd, true);
    audio.Play(ambient_snd);
  }

  // Camera

  float    dov     {cfg.Get<float>("cam_dov")};
  float    fov     {cfg.Get<float>("cam_fov")};
  float    near_z  {cfg.Get<float>("cam_nearz")};
  float    far_z   {cfg.Get<float>("cam_farz")};
  Vector   cam_pos {cfg.Get<Vector>("cam_pos")};
  Vector   cam_dir {cfg.Get<Vector>("cam_dir")};

  CameraMan camman {
    fov, dov, kWinWidth, kWinHeight, cam_pos, cam_dir, near_z, far_z, trig};
  camman.SetButton(CamAction::STRAFE_LEFT, KbdBtn::A);
  camman.SetButton(CamAction::STRAFE_RIGHT, KbdBtn::D);
  camman.SetButton(CamAction::MOVE_FORWARD, KbdBtn::W);
  camman.SetButton(CamAction::MOVE_BACKWARD, KbdBtn::S);
  camman.SetButton(CamAction::MOVE_UP, KbdBtn::R);
  camman.SetButton(CamAction::MOVE_DOWN, KbdBtn::F);
  camman.SetButton(CamAction::JUMP, KbdBtn::SPACE);
  camman.SetButton(CamAction::ZOOM_IN, KbdBtn::NUM9);
  camman.SetButton(CamAction::ZOOM_OUT, KbdBtn::NUM0);
  camman.SetButton(CamAction::WIRED, KbdBtn::T, 20);
  camman.SetButton(CamAction::SWITCH_TYPE, KbdBtn::ENTER, 20);
  camman.SetButton(CamAction::SPEED_UP, KbdBtn::LSHIFT);

  camman.SetState(CamState::FLY_MODE, true);
  camman.SetState(CamState::ON_GROUND, false);

  camman.SetValue(CamValue::OPERATOR_HEIGHT, cfg.Get<float>("cam_height"));
  camman.SetValue(CamValue::SPEED_UP, cfg.Get<float>("cam_speed_up"));

  Dynamics dyn_camera {
    cfg.Get<float>("cam_accel"),
    cfg.Get<float>("cam_frict"),
    cfg.Get<float>("cam_gravity"),
    cfg.Get<float>("cam_max_speed")
  };
  camman.SetDynamics(std::move(dyn_camera));  

  // Create jeep model

  const char*  kFname     {"../00_data/objects/jeep_front.ply"};
  const Vector kScale     {9.0f, 9.0f, 9.0f};
  const Vector kPosition  {cfg.Get<Vector>("cam_pos")};
  const Vector kRotate    {-90.0f, 0.0f, 0.0f};
  
  GlObject model (kFname, kPosition);
  object::Scale(model, kScale);
  object::Rotate(model, kRotate, trig);

  // Create player entity

  const float  kPlayerHeight {5.0f};
  const Vector kPlayerDir    {0.0f, 125.0f, 0.0f};
  const Vector kPlayerPos    {cfg.Get<Vector>("cam_pos")};
  Player jeep (std::move(model), kPlayerHeight, kPlayerPos, kPlayerDir, trig);

  // Set player controls

  jeep.SetButton(ObjAction::TURN_LEFT, KbdBtn::LEFT);
  jeep.SetButton(ObjAction::TURN_RIGHT, KbdBtn::RIGHT);
  jeep.SetButton(ObjAction::LOOK_UP, KbdBtn::N);
  jeep.SetButton(ObjAction::LOOK_DOWN, KbdBtn::M);
  jeep.SetButton(ObjAction::MOVE_FORWARD, KbdBtn::UP);
  jeep.SetButton(ObjAction::MOVE_BACKWARD, KbdBtn::DOWN);
  jeep.SetButton(ObjAction::SPEED_UP, KbdBtn::LCTRL);

  jeep.SetState(ObjState::FLY_MODE, false);
  jeep.SetValue(ObjValue::SPEED_UP, cfg.Get<float>("cam_speed_up"));  

  jeep.SetDirection(
    Player::PITCH, cfg.Get<float>("pl_pitch_red"),
    cfg.Get<float>("pl_pitch_vel"), cfg.Get<float>("pl_pitch_min"),
    cfg.Get<float>("pl_pitch_max"), cfg.Get<bool>("pl_pitch_lock")
  );
  jeep.SetDirection(
    Player::ROLL, cfg.Get<float>("pl_roll_red"),
    cfg.Get<float>("pl_roll_vel"), cfg.Get<float>("pl_roll_min"),
    cfg.Get<float>("pl_roll_max"), cfg.Get<bool>("pl_roll_lock")
  );
  jeep.SetDirection(
    Player::YAW, cfg.Get<float>("pl_yaw_red"),
    cfg.Get<float>("pl_yaw_vel"), cfg.Get<float>("pl_yaw_min"),
    cfg.Get<float>("pl_yaw_max"), cfg.Get<bool>("pl_yaw_lock")
  );
  Dynamics dyn_player {
    cfg.Get<float>("cam_accel"),
    cfg.Get<float>("cam_frict"),
    cfg.Get<float>("cam_gravity"),
    cfg.Get<float>("cam_max_speed")
  };
  jeep.SetDynamics(std::move(dyn_player));

  // Set initial camera position (by using orient_z vector)

  const Vector kDirOffset {6.5f, 0.0f, 0.0f};
  const Vector kVrpOffset {0.0f, 2.5f, -3.0f};

  camman.UseCamera(CamType::Follow::type);
  auto& follow_cam = camman.GetCamera(CamType::Follow::type);
  follow_cam.SetDirection(GlCamera::PITCH, 1.2f, 0.0f, -30.0f, 30.0f, false);
  follow_cam.SetDirection(GlCamera::ROLL, 1.2f, 0.0f, -10.0f, 10.0f, false);
  follow_cam.SetDirection(GlCamera::YAW, 1.0f, 0.0f, 0.0f, 0.0f, false);
  follow_cam.FollowFor(jeep, kVrpOffset, kDirOffset);

  // Create skybox

  Skybox  skybox (
    cfg.Get<std::string>("ter_sky").c_str(),
    cfg.Get<Vector>("cam_pos")
  );
  object::Scale(skybox, {far_z, far_z, far_z});
  object::Rotate(skybox, {90.0f, 0.0f, 0.0f}, trig);
  
  // Create terrain

  Terrain terrain (
    cfg.Get<std::string>("ter_hm").c_str(),
    cfg.Get<std::string>("ter_tx").c_str(),
    cfg.Get<float>("ter_divider"),
    cfg.Get<float>("ter_chunk"),
    static_cast<Shading>(cfg.Get<float>("ter_shading"))
  );
  terrain.SetDetalization(cfg.Get<Config::V_Float>("ter_detaliz"));
  auto& terrain_chunks = terrain.GetChunks();

  // Create nature objects
  

  Nature nature (cfg.Get<std::string>("ter_objs"), terrain, trig);
  Nature::ObjsList<NatureTypes> nature_list {
    { NatureTypes::TREE_T1, "../00_data/nature/tree_type_2.ply"},
    { NatureTypes::ROCK_T1, "../00_data/nature/rock_type_1.ply"},
    { NatureTypes::GRASS_T1, "../00_data/nature/grass_type_2.ply"}
  };
  nature.SetObjects<NatureTypes>(nature_list);

  nature.RecognizeObjects();
  auto& nature_objs = nature.GetObjects();

  // Make water
  
  Water water {
    terrain.GetHmWidth() * 2,
    cfg.Get<float>("ter_water_lvl"),
    color::fOceanBlue,
    Shading::GOURAUD
  };

  // Create render context

  RenderContext render_ctx(kWinWidth, kWinHeight, color::Black);
  render_ctx.is_zbuf_  = true;
  render_ctx.is_wired_ = false;
  render_ctx.is_alpha_ = true;
  render_ctx.is_bifiltering_ = false;
  render_ctx.is_mipmapping_ = true;
  render_ctx.mipmap_dist_ = 240.0f;
  render_ctx.clarity_  = cfg.Get<float>("cam_clarity");
  
  // Make triangles arrays

  auto tris_base = triangles::MakeBaseContainer(0);
  auto tris_ptrs = triangles::MakePtrsContainer(0);
  auto tris_sky  = triangles::MakeBaseContainer(0);

  // Prepare lights sources

  Lights      lights_all {};
  ColorTable  color_table {};

  auto   color   = color_table[cfg.Get<std::string>("light_amb_color")];
  auto   intense = cfg.Get<float>("light_amb_int");
  Vector lpos {};
  Vector ldir {};

  lights_all.AddAmbient(color, intense);

  color   = color_table[cfg.Get<std::string>("light_inf_color")];
  intense = cfg.Get<float>("light_inf_int");
  ldir    = cfg.Get<Vector>("light_inf_dir");

  if (intense)
    lights_all.AddInfinite(color, intense, ldir);

  color   = color_table[cfg.Get<std::string>("light_pnt_color")];
  intense = cfg.Get<float>("light_pnt_int");
  ldir    = cfg.Get<Vector>("light_pnt_dir"); 
  lpos    = cfg.Get<Vector>("light_pnt_pos");

  if (intense)
    lights_all.AddPoint(color, intense, lpos, ldir);

  Lights lights_sky {};

  color    = color_table[cfg.Get<std::string>("light_sky_color")];
  intense  = cfg.Get<float>("light_sky_int");
  
  lights_sky.AddAmbient(color, intense);
  
  // Main loop

  do {
    timer.Start();
    win.Clear();

    auto& cam = camman.GetCurrentCamera();

    // Process system input

    auto kbtn = win.ReadKeyboardBtn(BtnType::KB_DOWN);
    helpers::HandlePause(kbtn, win);
    helpers::HandleFullscreen(kbtn, mode, win);

    // Toggle flashlights

    if (kbtn == Btn::L && !lights_all.point_.empty())
    {
      lights_all.point_.back().intense_ = 0.0f;
      lights_all.infinite_.back().intense_ = cfg.Get<float>("light_inf_int") * 3;
    }
    else if (kbtn == Btn::K && !lights_all.point_.empty())
    {
      lights_all.point_.back().intense_ = cfg.Get<float>("light_pnt_int");
      lights_all.infinite_.back().intense_ = cfg.Get<float>("light_inf_int");
    }

    // Process gameplay input 

    camman.ProcessInput(win);
    jeep.ProcessInput(win);
    jeep.ProcessMovement(terrain);
    follow_cam.FollowFor(jeep);
    object::Translate(jeep, jeep.world_pos_);
    object::ResetAttributes(jeep);
    object::ComputeFaceNormals(jeep, true);
    auto hidden = object::RemoveHiddenSurfaces(jeep, cam);

    // Process player sounds

    auto range_1  = std::make_pair(0.0f, jeep.GetDynamics().GetMaxSpeed());
    auto range_2  = audio.GetModifierRange(engine_snd, engine_mod);
    auto curr_vel = jeep.GetDynamics().GetCurrentSpeed();
    auto modifier_val = math::LeadToRange(range_1, range_2, curr_vel);
    audio.SetModifierValue(engine_snd, engine_mod, modifier_val);

    // Process skybox
  
    skybox.world_pos_ = cam.vrp_;
    skybox.SetCoords(Coords::TRANS);
    skybox.CopyCoords(Coords::LOCAL, Coords::TRANS);
    object::Translate(skybox, skybox.world_pos_);
    object::ResetAttributes(skybox);
    hidden = object::RemoveHiddenSurfaces(skybox, cam);
    light::Object(skybox, lights_sky);

    // Process water

    water.SetCoords(Coords::TRANS);
    water.CopyCoords(Coords::LOCAL, Coords::TRANS);
    object::Translate(water, water.world_pos_);
    object::ResetAttributes(water);
    object::ComputeFaceNormals(water, true);
    object::RemoveHiddenSurfaces(water, cam);
    object::CullZ(water, cam, trig);
    object::CullX(water, cam, trig);
    object::CullY(water, cam, trig);
    object::VerticesNormals2Camera(water, cam, trig);

    // Process nature

    int obj_culled {};
    for (auto& obj : nature_objs) {
      obj.SetCoords(Coords::TRANS);
      obj.CopyCoords(Coords::LOCAL, Coords::TRANS);
      object::Translate(obj, obj.world_pos_);
      object::ResetAttributes(obj);
      object::ComputeFaceNormals(obj, true);
      object::RemoveHiddenSurfaces(obj, cam);
      obj_culled += object::CullZ(obj, cam, trig);
      obj_culled += object::CullX(obj, cam, trig);
      obj_culled += object::CullY(obj, cam, trig);
    }

    // Cull terrain chunks by world pos

    for (auto& chunk : terrain_chunks)
    {
      object::ResetAttributes(chunk);
      obj_culled += object::CullZ(chunk, cam, trig);
      obj_culled += object::CullX(chunk, cam, trig);
      obj_culled += object::CullY(chunk, cam, trig);
    }

    // Change terrain detalization

    terrain.ProcessDetalization(cam);
    
    for (auto& chunk : terrain_chunks)
    {
      if (!chunk.active_)
        continue;

      chunk.CopyCoords(Coords::LOCAL, Coords::TRANS);
      chunk.SetCoords(Coords::TRANS);

      object::ComputeFaceNormals(chunk, true);
      hidden += object::RemoveHiddenSurfaces(chunk, cam);
      object::VerticesNormals2Camera(chunk, cam, trig);
    }

    // Go to camera coords for light and skybox (exclude terrain)

    object::World2Camera(skybox, cam, trig);

    // Make triangles from terrain

    tris_base.resize(0);
    tris_ptrs.resize(0);
    tris_sky.resize(0);
    
    for (auto& chunk : terrain_chunks)
    {
      if (chunk.active_)
        triangles::AddFromObject(chunk, tris_base);
    }
    triangles::AddFromObject(water, tris_base);
    triangles::AddFromObjects(nature_objs, tris_base);
    triangles::AddFromObject(jeep, tris_base);
    triangles::World2Camera(tris_base, cam, trig);
    auto tri_culled = triangles::CullAndClip(tris_base, cam);
    
    // Light terrain triangles in world coordinates

    triangles::ComputeNormals(tris_base);

    // Convert all lights but point to camera coordinates

    light::World2Camera(lights_all, cam, trig);
    if (!lights_all.point_.empty())
      lights_all.point_.front().Reset();

    light::Triangles(tris_base, lights_all);
    light::Reset(lights_all);

    // Make triangles for skybox (we want light it sepearately)

    triangles::AddFromObject(skybox, tris_sky);
    tri_culled += triangles::CullAndClip(tris_sky, cam);
    triangles::AddFromTriangles(tris_sky, tris_base);

    // Triangles merging

    triangles::MakePointers(tris_base, tris_ptrs);
    triangles::SortZAvg(tris_ptrs);

    triangles::Camera2Persp(tris_base, cam);
    triangles::Persp2Screen(tris_base, cam);

    // Draw triangles

    render_ctx.is_wired_ = camman.GetState(CamState::WIRED_MODE);
    render::Context(tris_ptrs, render_ctx);
    fps.Count();

    win.Render();
    timer.Wait();

    if (kDebugShow && fps.Ready())
    {
      std::cerr << "Frames per second: " << fps.ReadPrev() << '\n';
      std::cerr << "Camera position: " << cam.vrp_ << '\n';
      std::cerr << "Camera direction: " << cam.dir_ << '\n';
      std::cerr << "Chunks total: " << terrain_chunks.size() << '\n';
      std::cerr << "Chunks culled: " << obj_culled << '\n';
      std::cerr << "Hidden surfaces: " << hidden << '\n';
      std::cerr << "Triangles total: " << tris_base.size() << '\n';
      std::cerr << "Triangles culled: " << tri_culled << '\n';   
      std::cerr << '\n';
    }

  } while (!win.Closed());

  return 0;
}