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
#include "lib/draw/extras/skybox.h"
#include "lib/draw/extras/terrain.h"
#include "lib/draw/extras/birds.h"
#include "lib/draw/extras/water.h"
#include "lib/system/timer.h"
#include "lib/system/fps_counter.h"
#include "lib/system/rand_toolkit.h"
#include "lib/math/trig.h"
#include "lib/math/matrix_rotate_uvn.h"
#include "lib/math/matrix_trans.h"

#include "../config.h"
#include "../helpers.h"
#include "../camera_operator.h"

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
  const bool kDebugShow {cfg.GetBool("dbg_show_info")};

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
  auto snd_ambient = cfg.GetString("snd_ambient");
  auto snd_steps = cfg.GetString("snd_steps");
  auto snd_jump = cfg.GetString("snd_jump");
  auto snd_landing = cfg.GetString("snd_landing");
  audio.LoadFx(snd_ambient, true);
  audio.LoadFx(snd_steps, false);
  audio.LoadFx(snd_jump, false);
  audio.LoadFx(snd_landing, false);

  audio.Play(snd_ambient);

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
  cam.SetJumpButton(KbdBtn::SPACE, 10);
  cam.SetSpeedUpButton(KbdBtn::LSHIFT);
  cam.SetSwitchTypeButton(KbdBtn::ENTER, 10);
  cam.SetZoomInButton(KbdBtn::NUM9);
  cam.SetZoomOutButton(KbdBtn::NUM0);
  cam.SetSwitchRollButton(KbdBtn::L, 10);
  cam.SetFlyModeButton(KbdBtn::NUM6, 10);
  cam.SetWiredModeButton(KbdBtn::T, 10);
  cam.SetOperatorHeight(cfg.GetFloat("cam_height"));
  cam.SetFlyMode(cfg.GetFloat("cam_fly_mode"));
  cam.SetOnGround(!cfg.GetFloat("cam_fly_mode"));
  cam.SetGravity(cfg.GetFloat("cam_gravity"));
  cam.SetJumpHeight(cfg.GetFloat("cam_jump"));
  cam.SetAcceleration(cfg.GetFloat("cam_accel"));
  cam.SetFriction(cfg.GetFloat("cam_frict"));
  cam.SetSpeedUpValue(cfg.GetFloat("cam_speed_up"));

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

  // Make water
  
  Water water {
    terrain.GetHmWidth() - 1,
    cfg.GetFloat("ter_water_lvl"),
    color::fOceanBlue,
    Shading::GOURAUD
  };

  // Create birds

  int kBirdChangeFly {rand_toolkit::get_rand(100,300)};
  std::vector<Bird> birds;
  for (int i = 0; i < 10; ++i)
  {
    auto bird_model = object::Make("../00_data/objects/bird.ply");
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
  render_ctx.clarity_  = cfg.GetFloat("cam_clarity");
  
  // Make triangles arrays

  auto tris_base = triangles::MakeBaseContainer(0);
  auto tris_ptrs = triangles::MakePtrsContainer(0);
  auto tris_sky  = triangles::MakeBaseContainer(0);

 // Prepare lights sources

  DayTime     day_time (-1.0f, 1.0f, 0.009f);
  Lights      lights_all {};
  ColorTable  color_table {};

  auto   color   = color_table[cfg.GetString("light_amb_color")];
  auto   intense = cfg.GetFloat("light_amb_int");
  Vector lpos {};
  Vector ldir {};

  lights_all.AddAmbient(color, intense);

  color   = color_table[cfg.GetString("light_inf_color")];
  intense = cfg.GetFloat("light_inf_int");
  ldir    = {-1.0f, -1.0f, 0.0f};

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
  
  // Lookat point

  Vector lookat_point {0.0f, 0.0f, 0.0f};

  // Main loop

  do {
    timer.Start();
    win.Clear();

    // Handle camera

    float ground = terrain.FindGroundPosition(cam.vrp_);
    cam.ProcessInput(win);
    cam.SetGroundPosition(ground);
    render_ctx.is_wired_ = cam.IsWired();

    // Handle sytem routines

    auto kbtn = win.ReadKeyboardBtn(BtnType::KB_DOWN);
    helpers::HandlePause(kbtn, win);
    helpers::HandleFullscreen(kbtn, mode, win);

    // Handle lookat point
    
    helpers::HandleObject(kbtn, lookat_point, cam_pos, cam_dir);
    if (cam.type_ == GlCamera::Type::UVN)
    {
      cam.LookAt(lookat_point);
      MatrixRotateUvn   mx_cam_rot    {cam.u_, cam.v_, cam.n_};
      cam.dir_ = coords::RotationMatrix2Euler(mx_cam_rot);
    }

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
      object::CullZ(bird, cam);
      object::CullX(bird, cam);
      object::CullY(bird, cam);
    }

    // Process water

    water.SetCoords(Coords::TRANS);
    water.CopyCoords(Coords::LOCAL, Coords::TRANS);
    object::ResetAttributes(water);
    object::ComputeFaceNormals(water, true);
    object::RemoveHiddenSurfaces(water, cam);
    object::Translate(water, water.world_pos_);
    object::CullZ(water, cam);
    object::CullX(water, cam);
    object::CullY(water, cam);
    object::VerticesNormals2Camera(water, cam);

    // Cull terrain chunks by world pos

    int obj_culled {};
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
    for (auto& bird : birds){
      if (bird.active_)
        triangles::AddFromObject(bird, tris_base);
    }
    triangles::AddFromObject(water, tris_base);
    triangles::World2Camera(tris_base, cam);
    auto tri_culled = triangles::CullAndClip(tris_base, cam);
    
    // Light terrain triangles in world coordinates

    triangles::ComputeNormals(tris_base);
    light::World2Camera(lights_all, cam);
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