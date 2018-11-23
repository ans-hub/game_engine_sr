// *************************************************************
// File:    skybox.cc
// Descr:   example of skybox (inside-textured cube)
// Author:  Novoselov Anton @ 2017
// *************************************************************

#include <iostream>
#include <sstream>
#include <iomanip> 

#include "lib/window/gl_window.h"
#include "lib/window/helpers.h"

#include "lib/render/gl_object.h"
#include "lib/render/gl_draw.h"
#include "lib/render/gl_triangle.h"
#include "lib/render/fx_colors.h"
#include "lib/render/gl_scr_buffer.h"
#include "lib/render/gl_z_buffer.h"

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
  const char* fname = ExtractFnameFromArgv(argc, argv);
  if (!fname) {
    std::cerr << "Incorrect file name\n";
    return 1;
  }
  
  TrigTable trig {};
  rand_toolkit::start_rand();

  FpsCounter fps {};
  constexpr int kFpsWait = 1000;
  Timer timer (kFpsWait);

  constexpr int kWinWidth = 800;
  constexpr int kWinHeight = 600;
  
  auto pos  = io_helpers::GetXYToMiddle(kWinWidth, kWinHeight); 
  GlWindow win (pos.x, pos.y, kWinWidth, kWinHeight, "Horizont"); 

  Skybox  horizont (fname, {0.0f, 0.0f, 0.0f});
  object::Scale(horizont, {200.0f, 200.0f, 200.0f});
  object::Rotate(horizont, {90.0f, 0.0f, 0.0f}, trig);

  float    dov     {1.0f};
  float    fov     {75.0f};
  Vector   cam_pos {0.0f, 0.0f, 0.0f};
  Vector   cam_dir {0.0f, 0.0f, 0.0f};
  float    near_z  {dov};
  float    far_z   {300};
  auto camman = MakeCameraman(
    fov, dov, kWinWidth, kWinHeight, cam_pos, cam_dir, near_z, far_z, trig);
  
  ScrBuffer buf (kWinWidth, kWinHeight, color::Black);
  ZBuffer   zbuf (kWinWidth, kWinHeight);

  horizont.SetCoords(Coords::LOCAL);
  object::Translate(horizont, horizont.world_pos_); 

  auto tris_base = triangles::MakeBaseContainer(0);
  auto tris_ptrs = triangles::MakePtrsContainer(0);

  using vector::operator<<;
  
  do {
    timer.Start();
    win.Clear();

    camman.ProcessInput(win);
    auto& cam = camman.GetCurrentCamera();
    auto kbtn = win.ReadKeyboardBtn(BtnType::KB_DOWN);
    helpers::HandlePause(kbtn, win);

    horizont.SetCoords(Coords::TRANS);
    horizont.CopyCoords(Coords::LOCAL, Coords::TRANS);
    object::ResetAttributes(horizont);

    auto hidden = object::RemoveHiddenSurfaces(horizont, cam);
    object::World2Camera(horizont, cam, trig);

    tris_base.resize(0);
    tris_ptrs.resize(0);
    triangles::AddFromObject(horizont, tris_base);
    auto culled = triangles::CullAndClip(tris_base, cam);
    triangles::MakePointers(tris_base, tris_ptrs);
    triangles::SortZAvg(tris_ptrs);
    triangles::Camera2Persp(tris_base, cam);
    triangles::Persp2Screen(tris_base, cam);

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