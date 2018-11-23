// *************************************************************
// File:    matrix.cc
// Descr:   matrix transformation example
// Author:  Novoselov Anton @ 2017
// *************************************************************

#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip> 

#include "lib/window/gl_window.h"
#include "lib/window/helpers.h"

#include "lib/system/timer.h"
#include "lib/system/fps_counter.h"
#include "lib/system/rand_toolkit.h"

#include "lib/math/vector.h"
#include "lib/math/segment.h"
#include "lib/math/trig.h"

#include "lib/render/gl_draw.h"
#include "lib/render/gl_text.h"
#include "lib/render/gl_coords.h"
#include "lib/render/gl_object.h"
#include "lib/render/cameras/gl_camera.h"

#include "lib/math/matrices/mx_rotate_eul.h"
#include "lib/math/matrices/mx_rotate_uvn.h"
#include "lib/math/matrices/mx_perspective.h"
#include "lib/math/matrices/mx_translate.h"
#include "lib/math/matrices/mx_view.h"
#include "lib/math/matrices/mx_scale.h"
#include "lib/math/matrices/mx_camera.h"

#include "../helpers.h"

using namespace anshub;
using namespace helpers;
using anshub::vector::operator<<;
using anshub::matrix::operator<<;

const char* HandleInput(int argc, const char** argv)
{
  if (argc != 2)
    return NULL;
  else
    return argv[1];
}

// Creates array of rectangles (w_cnt * h_cnt size) 

auto CreateGround(int rect_cnt, TrigTable& trig)
{
  auto master = object::Make("../00_data/objects/floor.ply", trig,
    {1.0f, 1.0f, 1.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f}
  );
  float item_size = 0.2f;
  float gap_size  = 5.0f;
  float step      = item_size + gap_size;
  float half_w    = (step) * (rect_cnt / 2);
  float half_h    = half_w;
  
  std::vector<GlObject> ground (rect_cnt * rect_cnt, master);
  auto it = ground.begin();
  for (auto z = -half_h; z < half_h; z += step)
  {
    for (auto x = -half_w; x < half_w; x += step)
    {
      Vector pos {x, 0.0f, z};
      it->world_pos_ = pos;
      object::Scale(*it, {item_size, item_size, item_size});      
      ++it;
    }
  }
  return ground;
}

void PrintInfo(
  GlText& text, FpsCounter& fps, 
  const Vector& obj_pos, const Vector& obj_rot, 
  const Vector& cam_pos, const Vector& cam_rot,
  int nfo_culled, int nfo_hidden)
{
  std::ostringstream oss {};
  
  oss << "FPS: " << fps.ReadPrev()
      << ", culled: " << nfo_culled << ", hidden: " << nfo_hidden;
  text.PrintString(60, 90, oss.str().c_str());

  oss.str("");
  oss.clear();
  oss << "Obj pos: " << obj_pos << ", cam pos: " << cam_pos;
  text.PrintString(60, 70, oss.str().c_str());

  oss.str("");
  oss.clear();
  oss << "Obj rot: " << obj_rot << ", cam rot: " << cam_rot;
  text.PrintString(60, 50, oss.str().c_str());
  
  oss.str("");
  oss.clear();
  oss << "Controls: W,S - forward/backward, ARROWS - move, Z-N - rotate";
  text.PrintString(60, 30, oss.str().c_str());  
}

int main(int argc, const char** argv)
{
  const char* fname = HandleInput(argc, argv);
  if (!fname) {
    std::cerr << "Incorrect file name\n";
    return 1;
  }

  // Math processor
  
  TrigTable trig {};
  rand_toolkit::start_rand();

  // Timers

  FpsCounter fps {};
  constexpr int kFpsWait = 250;
  Timer timer (kFpsWait);

  // Window

  constexpr int kWidth = 800;
  constexpr int kHeight = 600;
  auto pos  = io_helpers::GetXYToMiddle(kWidth, kHeight); 
  GlWindow win (pos.x, pos.y, kWidth, kHeight, "Camera"); 

  // Object

  auto obj = object::Make(
    fname, trig, 
    {1.0f, 1.0f, 1.0f},     // initial scale
    {0.0f, 0.0f, 10.0f},    // world pos
    {180.0f, 0.0f, 0.0f}    // initial rotate
  );
  auto ground = CreateGround(20, trig);

  // Camera

  float    dov     {2};
  float    fov     {75};
  Vector   cam_pos {-1.0f, 2.0f, 0.0f};
  Vector   cam_dir {0.0f, 0.0f, 0.0f};
  float    near_z  {dov};
  float    far_z   {500};
  auto camman = MakeCameraman(
    fov, dov, kWidth, kHeight, cam_pos, cam_dir, near_z, far_z, trig);

  // Other stuff

  ScrBuffer buf (kWidth, kHeight, 0);
  GlText text {win};
  
  Vector  obj_rot    {0.0f, 0.0f, 0.0f};
  Pos     mpos_prev {win.ReadMousePos()}; // to calc mouse pos between frames
  int     nfo_culled;                     // shown how much objects is culled
  int     nfo_hidden;                     // how much hidden surfaces removed

  do {
    timer.Start();
    win.Clear();

    // Handle input

    auto    kbtn = win.ReadKeyboardBtn(BtnType::KB_DOWN);
    auto    mbtn_pr = win.ReadMouseBtn(BtnType::MS_DOWN);
    auto    mbtn_rl = win.ReadMouseBtn(BtnType::MS_UP);
    if (mbtn_pr == Btn::LMB)
      camman.SetState(CamState::ROLL_MODE, true);
    if (mbtn_rl == Btn::LMB)
      camman.SetState(CamState::ROLL_MODE, false);      

    // Prepare to get input

    auto&   cam = camman.GetCurrentCamera();
    Vector  obj_vel    {0.0f, 0.0f, 0.0f};
    Vector  obj_scale  {1.0f, 1.0f, 1.0f};

    // Get input

    camman.ProcessInput(win);
    HandlePause(kbtn, win);
    HandleObject(kbtn, obj_vel, obj_rot, obj_scale);

    // Handle camera type

    if (cam.type_ == CamType::UVN)
      camman.GetCamera(CamType::Uvn::type).LookAt(obj.world_pos_);

    // Change object`s world position

    obj.world_pos_ += obj_vel;

    // Prepare transformation matrices for main object

    MatrixRotateEul   mx_rot {obj_rot, trig};
    MatrixTranslate   mx_trans {obj.world_pos_};
    MatrixPerspective mx_per {cam.dov_, cam.ar_};
    MatrixScale       mx_scale(obj_scale);

    // Prepare total matrix

    Matrix<4,4>       mx_total {};
    matrix::MakeIdentity(mx_total);

    // Rotate local coordinates of main object

    obj.SetCoords(Coords::LOCAL);
    object::ApplyMatrix(mx_rot, obj);
    object::RefreshOrientation(obj, mx_rot);
    obj.CopyCoords(Coords::LOCAL, Coords::TRANS);

    // Transform trans coordinates of main object

    mx_total = matrix::Multiplie(mx_total, mx_trans);
    mx_total = matrix::Multiplie(mx_total, mx_scale);
    obj.SetCoords(Coords::TRANS);
    object::ApplyMatrix(mx_total, obj);
    
    // Prepare camera`s matrices (Euler or uvn) for all objects

    MatrixCamera mx_cam {};
    if (cam.type_ == CamType::EULER)
    {
      MatrixTranslate   mx_cam_trans  {cam.vrp_ * (-1)};
      MatrixRotateEul   mx_cam_rot    {cam.dir_ * (-1), trig};
      mx_cam = matrix::Multiplie(mx_cam, mx_cam_trans);
      mx_cam = matrix::Multiplie(mx_cam, mx_cam_rot);
    }
    else
    {
      auto& uvn = camman.GetCamera(CamType::Uvn::type);      
      uvn.LookAt(obj.world_pos_);
      MatrixTranslate   mx_cam_trans  {cam.vrp_ * (-1)};
      MatrixRotateUvn   mx_cam_rot    {uvn.GetU(), uvn.GetV(), uvn.GetN()};
      mx_cam = matrix::Multiplie(mx_cam_trans, mx_cam_rot);
    }

    // Translate ground

    for (auto& item : ground)
    {
      item.CopyCoords(Coords::LOCAL, Coords::TRANS);      
      item.SetCoords(Coords::TRANS);
      object::Translate(item, item.world_pos_);
    }
 
    // Cull hidden surfaces

    object::ResetAttributes(obj);
    auto culled = object::Cull(obj, cam, mx_cam);
    nfo_hidden  = object::RemoveHiddenSurfaces(obj, cam);
    nfo_culled  = static_cast<int>(culled);
    
    objects::ResetAttributes(ground);
    nfo_culled += objects::Cull(ground, cam, mx_cam);
    nfo_hidden += objects::RemoveHiddenSurfaces(ground, cam);

    // Prepare drawing normals
    
    auto norms = object::ComputeDrawableVxsNormals(obj, 0.4f);
    coords::World2Camera(norms, cam.vrp_, cam.dir_, trig);
    coords::Camera2Persp(norms, cam.dov_, cam.ar_);
    coords::Persp2Screen(norms, cam.wov_, buf.Width(), buf.Height());
    
    // Go from world coords to camera, and the perspective coords

    matrix::MakeIdentity(mx_total);
    mx_total = matrix::Multiplie(mx_total, mx_cam);
    mx_total = matrix::Multiplie(mx_total, mx_per);
    object::ApplyMatrix(mx_total, obj);

    // Make the same for the net
    
    objects::ApplyMatrix(mx_total, ground);

    // Since after mx_per we have homogenous coords

    object::Homogenous2Normal(obj);
    objects::Homogenous2Normal(ground);

    // Get screen coordinates

    MatrixViewport mx_view {cam.wov_, cam.scr_w_, cam.scr_h_};
    object::ApplyMatrix(mx_view, obj);
    objects::ApplyMatrix(mx_view, ground);

    // Draw triangles (stored in object)

    buf.Clear();
    for (const auto& it : ground)
      draw_object::Wired(it, buf);
    draw_object::Wired(obj, buf);
    
    buf.SendDataToFB();

    // Print fps ans other info
    
    PrintInfo(
      text, fps, obj.world_pos_, obj_rot, cam.vrp_, cam.dir_, nfo_culled, nfo_hidden
    );
    fps.Count();

    win.Render();
    timer.Wait();

  } while (!win.Closed());

  return 0;
}