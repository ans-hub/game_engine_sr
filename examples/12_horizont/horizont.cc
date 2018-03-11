// *************************************************************
// File:    horizont.cc
// Descr:   example of horizont (inside-textured cube)
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

  // Horizont object

  auto horizont = object::Make(
    fname, trig,
    {10.0f, 10.0f, 10.0f},  // initial scale
    {0.0f, 0.0f, 0.0f},     // world pos
    {0.0f, 0.0f, 0.0f}      // initial rotate
  );

  // Camera

  float    dov     {2};
  float    fov     {60};
  Vector   cam_pos {0.0f, 0.0f, 0.0f};
  Vector   cam_dir {0.0f, 0.0f, 0.0f};
  float    near_z  {dov};
  float    far_z   {300};
  GlCamera cam (fov, dov, kWinWidth, kWinHeight, cam_pos, cam_dir, near_z, far_z);
  
  // Other stuff

  Buffer  buf (kWinWidth, kWinHeight, color::Black);
  ZBuffer zbuf (kWinWidth, kWinHeight);

  // Prepare horizont cube

  horizont.SetCoords(Coords::LOCAL);
  object::Translate(horizont, horizont.world_pos_); 

  // Main loop

  do {
    timer.Start();
    win.Clear();

    // Handle input

    auto kbtn = win.ReadKeyboardBtn(BtnType::KB_DOWN);
    helpers::HandleCamMovement(kbtn, cam);
    helpers::HandlePause(kbtn, win);

    // Draw horizont

    horizont.SetCoords(Coords::TRANS);
    horizont.CopyCoords(Coords::LOCAL, Coords::TRANS);
    object::ResetAttributes(horizont);
    object::RemoveHiddenSurfaces(horizont, cam);
    object::World2Camera(horizont, cam);    
    auto tris = triangles::MakeContainer();
    triangles::AddFromObject(horizont, tris);
    triangles::SortZAvgInv(tris);
    triangles::Camera2Persp(tris, cam);
    triangles::Homogenous2Normal(tris);
    triangles::Persp2Screen(tris, cam);

    // Draw triangles

    buf.Clear();
    zbuf.Clear();
    draw_triangles::Solid(tris, zbuf, buf);
    buf.SendDataToFB();
    fps.Count();

    win.Render();
    timer.Wait();    

    helpers::PrintFpsOnCmd(fps);
    
  } while (!win.Closed());

  return 0;
}