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

const char* ExtractFnameFromArgv(int argc, const char** argv)
{
  if (argc != 2)
    return NULL;
  else
    return argv[1];
}

int main(int argc, const char** argv)
{
  // Check file name

  const char* skybox_fname = ExtractFnameFromArgv(argc, argv);
  if (!skybox_fname) {
    std::cerr << "Incorrect file name\n";
    return 1;
  }
  
  // Math processor
  
  TrigTable trig {};
  rand_toolkit::start_rand();

  // Timers

  FpsCounter fps {};
  constexpr int kFpsWait = 1000;
  Timer timer (kFpsWait);

  // Constants

  constexpr int kWinWidth = 800;
  constexpr int kWinHeight = 600;
  const char* terrain_hm = "../00_data/earth/terrain_hm.bmp";
  const char* terrain_tx = "../00_data/earth/terrain_tx.bmp";
  constexpr int kDivTerrain {10};
  
  // Window
  
  auto pos  = io_helpers::GetXYToMiddle(kWinWidth, kWinHeight); 
  GlWindow win (pos.x, pos.y, kWinWidth, kWinHeight, "Terrain"); 

  // Camera

  float    dov     {1.0f};
  float    fov     {75.0f};
  Vector   cam_pos {-17.0f, 32.0f, 19.0f};
  Vector   cam_dir {0.0f, 0.0f, 0.0f};
  float    near_z  {dov};
  float    far_z   {300};
  GlCamera cam (fov, dov, kWinWidth, kWinHeight, cam_pos, cam_dir, near_z, far_z);
  Pos      mpos_prev {win.ReadMousePos()}; // to calc mouse pos between frames

  // Create skybox and terrain objects

  Skybox  skybox (skybox_fname, cam_pos);
  object::Scale(skybox, {200.0f, 200.0f, 200.0f});
  object::Rotate(skybox, {90.0f, 0.0f, 0.0f}, trig);

  Terrain terrain (terrain_hm, terrain_tx, kDivTerrain);
  object::Scale(terrain, {2.0f, 2.0f, 2.0f});
  
  // Other stuff

  Buffer  buf (kWinWidth, kWinHeight, color::Black);
  ZBuffer zbuf (kWinWidth, kWinHeight);
  
  // Make triangles arrays

  auto tris_base = triangles::MakeBaseContainer(0);
  auto tris_ptrs = triangles::MakePtrsContainer(0);

  // Main loop

  do {
    timer.Start();
    win.Clear();

    // Handle input

    auto kbtn = win.ReadKeyboardBtn(BtnType::KB_DOWN);
    auto mpos = win.ReadMousePos();
    helpers::HandleCamMovement(kbtn, cam);
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
    object::World2Camera(skybox, cam);

    // Draw terrain

    terrain.SetCoords(Coords::TRANS);
    terrain.CopyCoords(Coords::LOCAL, Coords::TRANS);
    object::ResetAttributes(terrain);    
    hidden += object::RemoveHiddenSurfaces(terrain, cam);
    object::World2Camera(terrain, cam);    

    // Make triangles

    tris_base.resize(0);
    tris_ptrs.resize(0);
    triangles::AddFromObject(skybox, tris_base);
    triangles::AddFromObject(terrain, tris_base);
    auto culled = triangles::CullAndClip(tris_base, cam);    
    triangles::MakePointers(tris_base, tris_ptrs);
    triangles::SortZAvg(tris_ptrs);
    triangles::Camera2Persp(tris_base, cam);
    triangles::Persp2Screen(tris_base, cam);

    // Draw triangles

    buf.Clear();
    zbuf.Clear();
    draw_triangles::Solid(tris_ptrs, zbuf, buf);
    buf.SendDataToFB();
    fps.Count();

    win.Render();
    timer.Wait();    

    PrintInfoOnCmd(fps, 0, hidden, cam.vrp_);

  } while (!win.Closed());

  return 0;
}