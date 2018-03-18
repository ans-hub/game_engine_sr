// *************************************************************
// File:    terrain.cc
// Descr:   example of terrain generation
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#include <iostream>
#include <sstream>
#include <iomanip> 

#include "lib/window/gl_window.h"
#include "lib/window/helpers.h"
#include "lib/draw/gl_object.h"
#include "lib/draw/gl_draw.h"
#include "lib/draw/gl_triangle.h"
#include "lib/draw/fx_colors.h"
#include "lib/draw/gl_lights.h"
#include "lib/draw/gl_buffer.h"
#include "lib/draw/gl_z_buffer.h"
#include "lib/draw/extras/terrain.h"
#include "lib/draw/extras/skybox.h"
#include "lib/system/timer.h"
#include "lib/system/fps_counter.h"
#include "lib/system/rand_toolkit.h"
#include "lib/math/trig.h"

#include "../helpers.h"

using namespace anshub;
using namespace helpers;

// Helper struct to recieve info about used file names

struct Fnames
{
  Fnames()
    : skybox_{nullptr}
    , terrain_hm_{nullptr}
    , terrain_tx_{nullptr}
    , divider_{} { } 
  Fnames(const char* sky, const char* terr_hm, const char* terr_tx, float div)
    : skybox_{sky}
    , terrain_hm_{terr_hm}
    , terrain_tx_{terr_tx}
    , divider_{div} { }
  const char* skybox_;        // skybox ply
  const char* terrain_hm_;    // terrain height map
  const char* terrain_tx_;    // terrain texture map
  float divider_;             // define how much we reduce heights

}; // struct Fnames

// Helpers struct for imitating of changing day and night

struct DayTime
{
  DayTime(float min_amb, float max_amb, float velocity)
    : min_amb_{std::min(0.0f, min_amb)}
    , max_amb_{std::max(1.0f, max_amb)}
    , velocity_{velocity} { }
  float NextTick(float intense)
  {
    float result {intense + velocity_};
    if (result < min_amb_ || result > max_amb_)
    {
      result -= velocity_;
      velocity_ *= -1.0f;
    }
    return result;
  }

  float min_amb_;               // min ambient lighting
  float max_amb_;               // max ambient lighting
  float velocity_;              // ambient change velocity

}; // struct DayTime

Fnames ExtractFnamesFromArgv(int argc, const char** argv)
{
  if (argc != 5)
    return {};
  else
    return {argv[1], argv[2], argv[3], std::stof(argv[4])};
}

// Imitates change day and night. Coordinate `w` inside vector used for
// decide - is we should add or sub 

void ProceedAmbientLightChange(Lights& lights, DayTime& dt)
{
  float& intense = lights.ambient_.back().intense_;
  intense = dt.NextTick(intense);
}

int main(int argc, const char** argv)
{
  // Check file name

  auto fnames = ExtractFnamesFromArgv(argc, argv);
  if (!fnames.skybox_ || !fnames.terrain_hm_ || !fnames.terrain_tx_)
  {
    std::cerr << "Incorrect file name\n";
    return 1;
  }
  
  // Math processor
  
  TrigTable trig {};
  rand_toolkit::start_rand();

  // Timers

  FpsCounter fps {};
  constexpr int kFpsWait {1000};
  Timer timer (kFpsWait);

  // Constants

  constexpr int kWinWidth {800};
  constexpr int kWinHeight {600};
  
  // Window
  
  auto pos  = io_helpers::GetXYToMiddle(kWinWidth, kWinHeight); 
  GlWindow win (pos.x, pos.y, kWinWidth, kWinHeight, "Terrain"); 

  // Camera

  float    dov     {5.0f};
  float    fov     {75.0f};
  Vector   cam_pos {-1.5f, 40.0f, 60.0f};
  Vector   cam_dir {55.0f, 15.0f, 0.0f};
  float    near_z  {dov};
  float    far_z   {800.0f};
  GlCamera cam (fov, dov, kWinWidth, kWinHeight, cam_pos, cam_dir, near_z, far_z);
  Pos      mpos_prev {win.ReadMousePos()}; // to calc mouse pos between frames

  // Create skybox and terrain objects

  Skybox  skybox (fnames.skybox_, cam.vrp_);
  object::Scale(skybox, {500.0f, 500.0f, 500.0f});
  object::Rotate(skybox, {90.0f, 0.0f, 0.0f}, trig);

  Terrain terrain (fnames.terrain_hm_, fnames.terrain_tx_, fnames.divider_);
  object::Scale(terrain, {2.0f, 2.0f, 2.0f});
  // terrain.SetDetalization({50.0f, 80.0f, 100.0f}, 1, 10);
  terrain.SetDetalization({1.0f}, 10, 10);
  terrain.shading_ = Shading::GOURANG;
  
  // Other stuff

  Buffer  buf (kWinWidth, kWinHeight, color::Black);
  ZBuffer zbuf (kWinWidth, kWinHeight);
  
  // Make triangles arrays

  auto tris_base = triangles::MakeBaseContainer(0);
  auto tris_ptrs = triangles::MakePtrsContainer(0);

  // Prepare lights sources
 
  DayTime day_time (0.1f, 0.7f, 0.0009f);

  FColor white  {255.0f, 255.0f, 255.0f};
  FColor yellow {255.0f, 255.0f, 0.0f};
  FColor blue   {0.0f, 0.0f, 255.0f};

  Lights lights_all {};
  lights_all.ambient_.emplace_back(white, 0.2f);
  lights_all.infinite_.emplace_back(white, 0.7f, Vector{-1.0f, -1.0f, 0.0f});

  Lights lights_sky {};
  lights_sky.ambient_.emplace_back(white, 0.7f);
  
  // Main loop

  do {
    timer.Start();
    win.Clear();

    // Handle input

    auto kbtn = win.ReadKeyboardBtn(BtnType::KB_DOWN);
    auto mpos = win.ReadMousePos();
    helpers::HandleCamMovement(kbtn, 1.0f, cam);
    helpers::HandleCamType(kbtn, cam);
    helpers::HandleCamRotate(false, mpos, mpos_prev, cam.dir_);
    helpers::HandlePause(kbtn, win);

    // Draw skybox
  
    skybox.world_pos_ = cam.vrp_;
    skybox.SetCoords(Coords::TRANS);
    skybox.CopyCoords(Coords::LOCAL, Coords::TRANS);
    object::ResetAttributes(skybox);
    auto hidden = object::RemoveHiddenSurfaces(skybox, cam);
    object::Translate(skybox, skybox.world_pos_);
    light::Object(skybox, lights_sky);

    // Draw terrain

    terrain.SetCoords(Coords::TRANS);
    terrain.CopyCoords(Coords::LOCAL, Coords::TRANS);
    terrain.UseDetalization(cam.vrp_);
    object::ResetAttributes(terrain);
    hidden += object::RemoveHiddenSurfaces(terrain, cam);

    // Light objects
    
    ProceedAmbientLightChange(lights_all, day_time);
    ProceedAmbientLightChange(lights_sky, day_time);

    // Go to camera coordinates

    object::World2Camera(skybox, cam);
    object::World2Camera(terrain, cam);
    light::World2Camera(lights_all, cam);
    object::ComputeFaceNormals(terrain);
    object::ComputeVertexNormalsV2(terrain);
    object::ComputeFaceNormals(terrain);

    // Make triangles from terrain

    tris_base.resize(0);
    tris_ptrs.resize(0);
    
    triangles::AddFromObject(terrain, tris_base);
    auto culled = triangles::CullAndClip(tris_base, cam);
    
    // Light terrain triangles

    light::Triangles(tris_base, lights_all);
    light::Reset(lights_all);

    // Make triangles for skybox (we want light it sepearately)

    auto tris_sky = triangles::MakeBaseContainer(32);
    triangles::AddFromObject(skybox, tris_sky);
    culled += triangles::CullAndClip(tris_sky, cam);

    // Triangles merging

    triangles::AddFromTriangles(tris_sky, tris_base);
    triangles::MakePointers(tris_base, tris_ptrs);
    triangles::SortZAvg(tris_ptrs);

    triangles::Camera2Persp(tris_base, cam);
    triangles::Persp2Screen(tris_base, cam);

    // Draw triangles

    // buf.Clear(); // we may don`t clean screen buffer since all pixels are redrawn
    zbuf.Clear();
    render::Solid(tris_ptrs, zbuf, 150.0f, buf);
    buf.SendDataToFB();
    fps.Count();

    win.Render();
    timer.Wait();    

    PrintInfoOnCmd(fps, 0, hidden, cam.vrp_);

  } while (!win.Closed());

  return 0;
}