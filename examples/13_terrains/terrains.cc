// *************************************************************
// File:    terrains.cc
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
#include "lib/extras/skybox.h"
#include "lib/extras/terrain.h"
#include "lib/extras/birds.h"
#include "lib/extras/water.h"
#include "lib/extras/cameraman.h"
#include "lib/system/timer.h"
#include "lib/system/fps_counter.h"
#include "lib/system/rand_toolkit.h"
#include "lib/math/trig.h"
#include "lib/math/matrix_rotate_uvn.h"
#include "lib/math/matrix_trans.h"
#include "lib/data/cfg_loader.h"

#include "../helpers.h"

using namespace anshub;
using namespace helpers;

// Helpers struct for imitating of changing day and night

struct DayTime
{
  DayTime(float min_amb, float max_amb, float velocity)
    : min_amb_{std::min(-1.0f, min_amb)}
    , max_amb_{std::max(1.0f, max_amb)}
    , velocity_{velocity} { }

  void ProceedInfiniteLightChange(Lights& lights)
  {
    auto dir = lights.infinite_.back().GetDirection();
    dir.x = this->NextTick(dir.x);
    lights.infinite_.back().SetDirection(dir);
  }

private:

  float NextTick(float intense)
  {
    float result {intense + velocity_};
    if (result < min_amb_ || result > max_amb_) {
      result -= velocity_;
      velocity_ *= -1.0f;
    }
    return result;
  }

  float min_amb_;               // min ambient lighting
  float max_amb_;               // max ambient lighting
  float velocity_;              // ambient change velocity

}; // struct DayTime

int main(int argc, const char** argv)
{
  using vector::operator<<;

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
  Timer timer (cfg.Get<float>("win_fps"));

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
  auto snd_ambient = cfg.Get<std::string>("snd_ambient");
  auto snd_steps = cfg.Get<std::string>("snd_steps");
  auto snd_jump = cfg.Get<std::string>("snd_jump");
  auto snd_landing = cfg.Get<std::string>("snd_landing");
  audio.LoadFx(snd_ambient, true);
  audio.LoadFx(snd_steps, false);
  audio.LoadFx(snd_jump, false);
  audio.LoadFx(snd_landing, false);

  audio.Play(snd_ambient);

  // Camera

  float    dov     {cfg.Get<float>("cam_dov")};
  float    fov     {cfg.Get<float>("cam_fov")};
  float    near_z  {cfg.Get<float>("cam_nearz")};
  float    far_z   {cfg.Get<float>("cam_farz")};
  Vector   cam_pos {cfg.Get<Vector>("cam_pos")};
  Vector   cam_dir {cfg.Get<Vector>("cam_dir")};
  
  CameraMan camman {
    fov, dov, kWinWidth, kWinHeight, cam_pos, cam_dir, near_z, far_z, trig
  };
  camman.SetButton(CamAction::STRAFE_LEFT, KbdBtn::A);
  camman.SetButton(CamAction::STRAFE_RIGHT, KbdBtn::D);
  camman.SetButton(CamAction::MOVE_FORWARD, KbdBtn::W);
  camman.SetButton(CamAction::MOVE_BACKWARD, KbdBtn::S);
  camman.SetButton(CamAction::MOVE_UP, KbdBtn::R);
  camman.SetButton(CamAction::MOVE_DOWN, KbdBtn::F);
  camman.SetButton(CamAction::JUMP, KbdBtn::SPACE);
  camman.SetButton(CamAction::ZOOM_IN, KbdBtn::NUM9);
  camman.SetButton(CamAction::ZOOM_IN, KbdBtn::NUM0);
  camman.SetButton(CamAction::ROLL_MODE, KbdBtn::L, 20);
  camman.SetButton(CamAction::FLY_MODE, KbdBtn::K, 20);
  camman.SetButton(CamAction::WIRED, KbdBtn::T, 20);
  camman.SetButton(CamAction::SWITCH_TYPE, KbdBtn::ENTER, 20);
  camman.SetButton(CamAction::SPEED_UP, KbdBtn::LSHIFT);

  camman.SetState(CamState::FLY_MODE, cfg.Get<bool>("cam_fly_mode"));
  camman.SetState(CamState::ON_GROUND, false);

  camman.SetValue(CamValue::OPERATOR_HEIGHT, cfg.Get<float>("cam_height"));
  camman.SetValue(CamValue::MOUSE_SENSITIVE, cfg.Get<float>("cam_mouse_sens"));
  camman.SetValue(CamValue::SPEED_UP, cfg.Get<float>("cam_speed_up"));
  camman.SetValue(CamValue::JUMP_HEIGHT, cfg.Get<float>("cam_jump"));

  Dynamics dyn {
    cfg.Get<float>("cam_accel"),
    cfg.Get<float>("cam_frict"),
    cfg.Get<float>("cam_gravity"),
    100.0f,
  };
  camman.SetDynamics(std::move(dyn));

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
  terrain.SetDetalization(cfg.Get<std::vector<float>>("ter_detaliz"));
  auto& terrain_chunks = terrain.GetChunks();

  // Make water
  
  Water water {
    terrain.GetHmWidth() - 1,
    cfg.Get<float>("ter_water_lvl"),
    color::fOceanBlue,
    Shading::GOURAUD
  };

  // Create birds

  int kBirdChangeFly {rand_toolkit::get_rand(100,300)};
  std::vector<Bird> birds;
  for (int i = 0; i < 10; ++i)
  {
    GlObject bird_model {
      "../00_data/objects/bird.ply", {0.0f, 0.0f, 0.0f}};
    birds.emplace_back(
      std::move(bird_model),
      kBirdChangeFly,
      terrain.GetHmWidth() * 4,
      terrain.GetHmWidth() * 4);
  }

  // Create render context

  RenderContext render_ctx(kWinWidth, kWinHeight, color::Black);
  render_ctx.is_zbuf_  = true;
  render_ctx.is_wired_ = false;
  render_ctx.is_alpha_ = true;
  render_ctx.is_bifiltering_ = false;
  render_ctx.clarity_  = cfg.Get<float>("cam_clarity");
  
  // Make triangles arrays

  auto tris_base = triangles::MakeBaseContainer(0);
  auto tris_ptrs = triangles::MakePtrsContainer(0);
  auto tris_sky  = triangles::MakeBaseContainer(0);

 // Prepare lights sources

  DayTime     day_time (-1.0f, 1.0f, 0.009f);
  Lights      lights_all {};
  ColorTable  color_table {};

  auto   color   = color_table[cfg.Get<std::string>("light_amb_color")];
  auto   intense = cfg.Get<float>("light_amb_int");
  Vector lpos {};
  Vector ldir {};

  lights_all.AddAmbient(color, intense);

  color   = color_table[cfg.Get<std::string>("light_inf_color")];
  intense = cfg.Get<float>("light_inf_int");
  ldir    = {-1.0f, -1.0f, 0.0f};

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
  
  // Lookat point

  Vector lookat_point {0.0f, 0.0f, 0.0f};

  // Main loop

  do {
    timer.Start();
    win.Clear();

    // Handle camera
    
    auto& cam = camman.GetCurrentCamera();
    float ground = terrain.FindGroundPosition(cam.vrp_);
    camman.SetGroundPosition(ground);
    camman.ProcessInput(win);
    render_ctx.is_wired_ = camman.GetState(CamState::WIRED_MODE);

    // Handle sytem routines

    auto kbtn = win.ReadKeyboardBtn(BtnType::KB_DOWN);
    helpers::HandlePause(kbtn, win);
    helpers::HandleFullscreen(kbtn, mode, win);

    // Handle lookat point
    
    helpers::HandleObject(kbtn, lookat_point, cam_pos, cam_dir);
    if (cam.type_ == CamType::UVN)
      camman.GetCamera(CamType::Uvn::type).LookAt(lookat_point);

    // Process light intense changing

    day_time.ProceedInfiniteLightChange(lights_all);
    
    // Process skybox
  
    skybox.world_pos_ = cam.vrp_;
    skybox.SetCoords(Coords::TRANS);
    skybox.CopyCoords(Coords::LOCAL, Coords::TRANS);
    object::ResetAttributes(skybox);
    auto hidden = object::RemoveHiddenSurfaces(skybox, cam);
    object::Translate(skybox, skybox.world_pos_);
    light::Object(skybox, lights_sky);

    // Process birds

    for (auto& bird : birds)
    {
      bird.Process();
      bird.SetCoords(Coords::TRANS);
      bird.CopyCoords(Coords::LOCAL, Coords::TRANS);
      object::ResetAttributes(bird);
      object::ComputeFaceNormals(bird, true);
      object::RemoveHiddenSurfaces(bird, cam);
      object::Translate(bird, bird.world_pos_);
      object::CullZ(bird, cam, trig);
      object::CullX(bird, cam, trig);
      object::CullY(bird, cam, trig);
    }

    // Process water

    water.SetCoords(Coords::TRANS);
    water.CopyCoords(Coords::LOCAL, Coords::TRANS);
    object::ResetAttributes(water);
    object::ComputeFaceNormals(water, true);
    object::RemoveHiddenSurfaces(water, cam);
    object::Translate(water, water.world_pos_);
    object::CullZ(water, cam, trig);
    object::CullX(water, cam, trig);
    object::CullY(water, cam, trig);
    object::VerticesNormals2Camera(water, cam, trig);

    // Cull terrain chunks by world pos

    int obj_culled {};
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
    for (auto& bird : birds){
      if (bird.active_)
        triangles::AddFromObject(bird, tris_base);
    }
    triangles::AddFromObject(water, tris_base);
    triangles::World2Camera(tris_base, cam, trig);
    auto tri_culled = triangles::CullAndClip(tris_base, cam);
    
    // Light terrain triangles in world coordinates

    triangles::ComputeNormals(tris_base);
    light::World2Camera(lights_all, cam, trig);
    if (!lights_all.point_.empty())     // we use point as flash light
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