// *************************************************************
// File:    skybox.cc
// Descr:   example of skybox (inside-textured cube)
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
#include "lib/draw/gl_scr_buffer.h"
#include "lib/draw/gl_z_buffer.h"

#include "lib/system/timer.h"
#include "lib/system/fps_counter.h"
#include "lib/system/rand_toolkit.h"

#include "lib/math/trig.h"

#include "lib/extras/skybox.h"

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

  const char* fname = ExtractFnameFromArgv(argc, argv);
  if (!fname) {
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
  
  // Window
  
  auto pos  = io_helpers::GetXYToMiddle(kWinWidth, kWinHeight); 
  GlWindow win (pos.x, pos.y, kWinWidth, kWinHeight, "Horizont"); 

  // Create skybox and terrain objects

  Skybox  horizont (fname, {0.0f, 0.0f, 0.0f});
  object::Scale(horizont, {200.0f, 200.0f, 200.0f});
  object::Rotate(horizont, {90.0f, 0.0f, 0.0f}, trig);

  // Camera

  float    dov     {1.0f};
  float    fov     {75.0f};
  Vector   cam_pos {0.0f, 0.0f, 0.0f};
  Vector   cam_dir {0.0f, 0.0f, 0.0f};
  float    near_z  {dov};
  float    far_z   {300};
  auto camman = MakeCameraman(
    fov, dov, kWinWidth, kWinHeight, cam_pos, cam_dir, near_z, far_z, trig);
  
  // Other stuff

  ScrBuffer buf (kWinWidth, kWinHeight, color::Black);
  ZBuffer   zbuf (kWinWidth, kWinHeight);

  // Prepare horizont cube

  horizont.SetCoords(Coords::LOCAL);
  object::Translate(horizont, horizont.world_pos_); 

  // Make triangles arrays

  auto tris_base = triangles::MakeBaseContainer(0);
  auto tris_ptrs = triangles::MakePtrsContainer(0);

  // Main loop
using vector::operator<<;
  do {
    timer.Start();
    win.Clear();

    // Handle input

    camman.ProcessInput(win);
    auto& cam = camman.GetCurrentCamera();
    auto kbtn = win.ReadKeyboardBtn(BtnType::KB_DOWN);
    helpers::HandlePause(kbtn, win);

    // Draw horizont

    horizont.SetCoords(Coords::TRANS);
    horizont.CopyCoords(Coords::LOCAL, Coords::TRANS);
    object::ResetAttributes(horizont);

    auto hidden = object::RemoveHiddenSurfaces(horizont, cam);
    object::World2Camera(horizont, cam, trig);

    // Make triangles

    tris_base.resize(0);
    tris_ptrs.resize(0);
    triangles::AddFromObject(horizont, tris_base);
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