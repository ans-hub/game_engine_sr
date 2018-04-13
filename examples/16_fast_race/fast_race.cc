// *************************************************************
// File:    fast_race.cc
// Descr:   example of terrain generation
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <iomanip> 

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
#include "lib/draw/extras/skybox.h"
#include "lib/draw/extras/terrain.h"
#include "lib/draw/extras/nature.h"
#include "lib/draw/extras/water.h"
#include "lib/system/timer.h"
#include "lib/system/fps_counter.h"
#include "lib/system/rand_toolkit.h"
#include "lib/math/trig.h"
#include "lib/math/matrix_rotate_eul.h"
#include "lib/math/matrix_rotate_uvn.h"
#include "lib/math/matrix_trans.h"

#include "../player.h"
#include "../config.h"
#include "../helpers.h"
#include "../camera_operator.h"

using namespace anshub;
using namespace helpers;

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
  Timer timer (cfg.GetFloat("win_fps"));

  // Constants

  const int kWinWidth {cfg.GetFloat("win_w")};
  const int kWinHeight {cfg.GetFloat("win_h")};

  FColor kWhite  {255.0f, 255.0f, 255.0f};
  FColor kYellow {255.0f, 255.0f, 0.0f};
  FColor kBlue   {0.0f, 0.0f, 255.0f};
  
  // Window
  
  auto pos  = io_helpers::GetXYToMiddle(kWinWidth, kWinHeight); 
  auto mode = io_helpers::FindVideoMode(kWinWidth, kWinHeight);
  GlWindow win (pos.x, pos.y, kWinWidth, kWinHeight, "Terrain");
  
  if (cfg.GetFloat("win_fs"))
    win.ToggleFullscreen(mode);

  // Audio

  AudioFx audio {};
  auto engine_snd = cfg.GetString("snd_engine");
  auto engine_mod = AudioFx::Modifier::PITCH;

  if (!engine_snd.empty())
  {
    auto min_pitch = cfg.GetFloat("snd_low_pitch");
    auto max_pitch = cfg.GetFloat("snd_high_pitch");
    audio.LoadFx(engine_snd, true);
    audio.SetModifierRange(engine_snd, engine_mod, min_pitch, max_pitch);
    audio.SetModifierValue(engine_snd, engine_mod, min_pitch);
    audio.PlayFx(engine_snd);
  }
  
  // Camera

  float    dov     {cfg.GetFloat("cam_dov")};
  float    fov     {cfg.GetFloat("cam_fov")};
  float    near_z  {cfg.GetFloat("cam_nearz")};
  float    far_z   {cfg.GetFloat("cam_farz")};
  Vector   cam_pos {cfg.GetVector3d("cam_pos")};
  Vector   cam_dir {cfg.GetVector3d("cam_dir")};
  
  CameraOperator cam {
    fov, dov, kWinWidth, kWinHeight, cam_pos, cam_dir, near_z, far_z
  };
  
  cam.SetLeftButton(KbdBtn::A);
  cam.SetRightButton(KbdBtn::D);
  cam.SetForwardButton(KbdBtn::W);
  cam.SetBackwardButton(KbdBtn::S);
  cam.SetUpButton(KbdBtn::R);
  cam.SetDownButton(KbdBtn::F);
  cam.SetZoomInButton(KbdBtn::NUM9);
  cam.SetZoomOutButton(KbdBtn::NUM0);
  cam.SetSwitchRollButton(KbdBtn::L, 10);
  cam.SetWiredModeButton(KbdBtn::T, 10);
  cam.SetSpeedUpButton(KbdBtn::LSHIFT);
  cam.SetOperatorHeight(cfg.GetFloat("cam_height"));
  cam.SetFlyMode(cfg.GetFloat("cam_fly_mode"));
  cam.SetOnGround(cfg.GetFloat("cam_fly_mode"));
  cam.SetGravity(cfg.GetFloat("cam_gravity"));
  cam.SetAcceleration(cfg.GetFloat("cam_accel"));
  cam.SetFriction(cfg.GetFloat("cam_frict"));
  cam.SetMaxSpeed(cfg.GetFloat("cam_max_speed"));  
  cam.SetSpeedUpValue(cfg.GetFloat("cam_speed_up"));
  cam.SetFlyMode(true);  

  // Create skybox

  Skybox  skybox (cfg.GetString("ter_sky").c_str(), cam.vrp_);
  object::Scale(skybox, {far_z, far_z, far_z});
  object::Rotate(skybox, {90.0f, 0.0f, 0.0f}, trig);
  
  // Create terrain

  Terrain terrain (
    cfg.GetString("ter_hm").c_str(),
    cfg.GetString("ter_tx").c_str(),
    cfg.GetFloat("ter_divider"),
    cfg.GetFloat("ter_chunk"),
    static_cast<Shading>(cfg.GetFloat("ter_shading"))
  );
  terrain.SetDetalization(cfg.GetVectorF("ter_detaliz"));
  auto& terrain_chunks = terrain.GetChunks();

  // Create nature objects

  Nature nature (cfg.GetString("ter_objs"), terrain, trig);
  nature.SetObjects(Nature::TREE_T1, "../00_data/nature/tree_type_1.ply");
  nature.SetObjects(Nature::ROCK_T1, "../00_data/nature/rock_type_1.ply");
  nature.RecognizeObjects();
  auto& nature_objs = nature.GetObjects();

  // Make water
  
  Water water {
    terrain.GetHmWidth() * 2,
    cfg.GetFloat("ter_water_lvl"),
    color::fOceanBlue,
    Shading::GOURAUD
  };

  // Create render context

  RenderContext render_ctx(kWinWidth, kWinHeight, color::Black);
  render_ctx.is_zbuf_  = true;
  render_ctx.is_wired_ = false;
  render_ctx.is_alpha_ = true;
  render_ctx.is_bifiltering_ = false;
  render_ctx.clarity_  = cfg.GetFloat("cam_clarity");
  
  // Make triangles arrays

  auto tris_base = triangles::MakeBaseContainer(0);
  auto tris_ptrs = triangles::MakePtrsContainer(0);
  auto tris_sky  = triangles::MakeBaseContainer(0);

  // Prepare lights sources

  Lights      lights_all {};
  ColorTable  color_table {};

  auto   color   = color_table[cfg.GetString("light_amb_color")];
  auto   intense = cfg.GetFloat("light_amb_int");
  Vector lpos {};
  Vector ldir {};

  lights_all.AddAmbient(color, intense);

  color   = color_table[cfg.GetString("light_inf_color")];
  intense = cfg.GetFloat("light_inf_int");
  ldir    = {0.0f, -1.0f, -1.0f};

  if (intense)
    lights_all.AddInfinite(color, intense, ldir);

  color   = color_table[cfg.GetString("light_pnt_color")];
  intense = cfg.GetFloat("light_pnt_int");
  ldir    = cfg.GetVector3d("light_pnt_dir"); 
  lpos    = cfg.GetVector3d("light_pnt_pos");

  if (intense)
    lights_all.AddPoint(color, intense, lpos, ldir);

  Lights lights_sky {};

  color    = color_table[cfg.GetString("light_sky_color")];
  intense  = cfg.GetFloat("light_sky_int");
  
  lights_sky.AddAmbient(color, intense);

  // Create jeep model

  const char*  kFname     {"../00_data/objects/jeep_front.ply"};
  const Vector kScale     {9.0f, 9.0f, 9.0f};
  const Vector kPosition  {cfg.GetVector3d("cam_pos")};
  const Vector kRotate    {-90.0f, 0.0f, 0.0f};
  auto model = object::Make(kFname, trig, kScale, kPosition, kRotate);

  // Create player entity

  const float  kPlayerHeight {5.0f};
  const float  kCameraHeight {2.5f};
  const Vector kPlayerDir    {0.0f, 125.0f, 0.0f};
  Player jeep (std::move(model), cam, kPlayerHeight, kCameraHeight, kPlayerDir);

  // Set player controls

  jeep.SetButton(Player::TURN_LEFT, KbdBtn::LEFT);
  jeep.SetButton(Player::TURN_RIGHT, KbdBtn::RIGHT);
  jeep.SetButton(Player::MOVE_FORWARD, KbdBtn::UP);
  jeep.SetButton(Player::MOVE_BACKWARD, KbdBtn::DOWN);
  jeep.SetButton(Player::LOOK_UP, KbdBtn::B);
  jeep.SetButton(Player::LOOK_DOWN, KbdBtn::N);
  jeep.SetButton(Player::SPEED_UP, KbdBtn::LSHIFT);
  jeep.SetFlyMode(false);
  cam.SetGravity(cfg.GetFloat("cam_gravity"));
  cam.SetAcceleration(cfg.GetFloat("cam_accel"));
  cam.SetFriction(cfg.GetFloat("cam_frict"));

  // Set initial camera position (by using orient_z vector)

  const float kCamOffsetZ {-3.0f};
  const float kCamPitch {6.5f};

  player::InitCamera(jeep, cam, kCamOffsetZ, kCamPitch);
  
  // Main loop

  do {
    timer.Start();
    win.Clear();

    // Process gameplay input 

    player::ProcessCameraInput(jeep, cam, win);
    player::ProcessPlayerInput(jeep, cam, win);
    player::ProcessGroundPosition(jeep, terrain);
    player::ProcessGroundDirection(jeep, cam, terrain);

    // Process system input

    auto kbtn = win.ReadKeyboardBtn(BtnType::KB_DOWN);
    helpers::HandlePause(kbtn, win);
    helpers::HandleFullscreen(kbtn, mode, win);

    // Process player transformation
    
    player::ProcessPlayerOrientation(jeep);
    player::ProcessPlayerRotating(jeep);
    player::ProcessCameraRotating(jeep, cam);
    object::Translate(jeep.obj_, jeep.obj_.world_pos_);
    object::ResetAttributes(jeep.obj_);
    object::ComputeFaceNormals(jeep.obj_, true);
    auto hidden = object::RemoveHiddenSurfaces(jeep.obj_, cam);

    // Process player sounds

    auto range_1  = std::make_pair(0.0f, jeep.max_speed_);
    auto range_2  = audio.GetModifierRange(engine_snd, engine_mod);
    auto curr_vel = jeep.curr_speed_;
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
    object::CullZ(water, cam);
    object::CullX(water, cam);
    object::CullY(water, cam);
    object::VerticesNormals2Camera(water, cam);

    // Process nature

    int obj_culled {};
    for (auto& obj : nature_objs) {
      obj.SetCoords(Coords::TRANS);
      obj.CopyCoords(Coords::LOCAL, Coords::TRANS);
      object::Translate(obj, obj.world_pos_);
      object::ResetAttributes(obj);
      object::ComputeFaceNormals(obj, true);
      object::RemoveHiddenSurfaces(obj, cam);
      obj_culled += object::CullZ(obj, cam);
      obj_culled += object::CullX(obj, cam);
      obj_culled += object::CullY(obj, cam);
      //objects::VerticesNormals2Camera(water, cam);
    }

    // Cull terrain chunks by world pos

    for (auto& chunk : terrain_chunks)
    {
      object::ResetAttributes(chunk);
      obj_culled += object::CullZ(chunk, cam);
      obj_culled += object::CullX(chunk, cam);
      obj_culled += object::CullY(chunk, cam);
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
      object::VerticesNormals2Camera(chunk, cam);
    }

    // Go to camera coords for light and skybox (exclude terrain)

    object::World2Camera(skybox, cam);

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
    triangles::AddFromObject(jeep.obj_, tris_base);
    triangles::World2Camera(tris_base, cam);
    auto tri_culled = triangles::CullAndClip(tris_base, cam);
    
    // Light terrain triangles in world coordinates

    triangles::ComputeNormals(tris_base);

    // auto& flash = lights_all.point_.front();
    // Vector pos {flash.GetPosition()};
    // Vector dir {flash.GetDirection()};
    
    // Convert all lights but point to camera coordinates

    light::World2Camera(lights_all, cam);
    if (!lights_all.point_.empty())
      lights_all.point_.front().Reset();

    // if (kbtn == Btn::NUM6)
    //   pos.y += 1.0f;
    // if (kbtn == Btn::NUM7)
    //   pos.y -= 1.0f;
    // if (kbtn == Btn::Y)
    //   pos.z += 1.0f;
    // if (kbtn == Btn::U)
    //   pos.z -= 1.0f;
    // if (kbtn == Btn::H)
    //   dir.y += 1.0f;
    // if (kbtn == Btn::J)
    //   dir.y -= 1.0f;
    // if (kbtn == Btn::B)
    //   dir.z += 1.0f;
    // if (kbtn == Btn::M)
    //   dir.z -= 1.0f;
    // if (kbtn == Btn::C)
    //   flash.intense_ += 1.0f;
    // if (kbtn == Btn::V)
    //   flash.intense_ -= 1.0f;

// std::cerr << pos << '\n';
// std::cerr << dir << '\n';
      
    // flash.SetPosition(pos);
    // flash.SetDirection(dir);

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

    render_ctx.is_wired_ = cam.IsWired();
    render::Context(tris_ptrs, render_ctx);
    fps.Count();

    win.Render();
    timer.Wait();
    
    if (fps.Ready())
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