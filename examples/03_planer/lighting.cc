// *************************************************************
// File:    mx_trans.cc
// Descr:   matrix transformation example
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
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
#include "lib/draw/gl_draw.h"
#include "lib/draw/gl_text.h"
#include "lib/draw/gl_object.h"
#include "lib/draw/gl_camera_euler.h"
#include "lib/math/matrix_rotate.h"
#include "lib/math/matrix_persp.h"
#include "lib/math/matrix_trans.h"
#include "lib/math/matrix_view.h"
#include "lib/math/matrix_scale.h"
#include "lib/math/matrix_camera.h"

using namespace anshub;
using anshub::vector::operator<<;
using anshub::matrix::operator<<;

void HandleCameraRotate(bool mode, const Pos& mpos, Pos& mpos_prev, Vector& ang)
{
  if (mode)
    ang.z += mpos_prev.x - mpos.x;
  else
    ang.y += mpos_prev.x - mpos.x;
  ang.x += mpos_prev.y - mpos.y;
  mpos_prev = mpos;
}

void HandleCameraPosition(Btn kbtn, Vector& pos)
{
  switch (kbtn)
  {
    case Btn::W : pos.z += 0.5; break; 
    case Btn::S : pos.z -= 0.5; break; 
    case Btn::A : pos.x -= 0.5; break; 
    case Btn::D : pos.x += 0.5; break; 
    case Btn::R : pos.y += 0.5; break; 
    case Btn::F : pos.y -= 0.5; break;
    default     : break;
  }
}

void HandleObject(Btn key, Vector& vel, Vector& rot, Vector& scale)
{
  switch(key)
  {
  //   case Btn::W :     vel.z =  0.5f; break;
  //   case Btn::S :     vel.z = -0.5f; break;
    case Btn::UP :    vel.y =  0.5f; break;
    case Btn::DOWN :  vel.y = -0.5f; break;
    case Btn::LEFT :  vel.x = -0.5f; break;
    case Btn::RIGHT : vel.x =  0.5f; break;
    case Btn::Z :     rot.x -= 0.5f; break;
    case Btn::X :     rot.x += 0.5f; break;
    case Btn::C :     rot.y -= 0.5f; break;
    case Btn::V :     rot.y += 0.5f; break;
    case Btn::B :     rot.z -= 0.5f; break;
    case Btn::N :     rot.z += 0.5f; break;
    case Btn::NUM1 :  scale.x += 0.02;
                      scale.y += 0.02;
                      scale.z += 0.02; break;
    case Btn::NUM2 :  scale.x -= 0.02;
                      scale.y -= 0.02;
                      scale.z -= 0.02; break;
    default : break;
  }
}

void HandlePause(Btn key, GlWindow& win)
{
  if (key == Btn::SPACE)
  {
    Timer timer {};
    timer.SetMillisecondsToWait(100);
    while (true)
    {
      if (win.ReadKeyboardBtn(BtnType::KB_DOWN) == Btn::SPACE)
        break;
      else
        timer.Wait();
    }
  }
}

const char* HandleInput(int argc, const char** argv)
{
  if (argc != 2)
    return NULL;
  else
    return argv[1];
}

void PrintInfo(
  GlText& text, const FpsCounter& fps, 
  const Vector& obj_pos, const Vector& obj_rot, 
  const Vector& cam_pos, const Vector& cam_rot)
{
  std::ostringstream oss {};
  
  oss << "FPS: " << std::setw(7) << std::left << fps.ReadPrev();
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

  Vector  obj_scale  {1.0f, 1.0f, 1.0f};
  Vector  obj_pos    {0.0f, 0.0f, 15.0f};
  Vector  obj_rot    {1.0f, 1.0f, 2.0f};
  GlObject  obj = object::Make(fname, obj_scale, obj_pos);  // master object

  std::vector<GlObject> v {};
  for (int i = 0; i < 10; ++i)
    v.push_back(obj);

  for (auto& obj : v)
  {
    float x = rand_toolkit::get_rand(-100, 100);
    float y = rand_toolkit::get_rand(-100, 100);
    float z = rand_toolkit::get_rand(-100, 100);
    obj.world_pos_= {x,y,z};
  }
  
  // Camera

  float   dov     {2};
  float   fov     {60};
  Vector  cam_pos {0.0f, 0.0f, 9.0f};
  Vector  cam_dir {0.0f, 0.0f, 0.0f};
  float   near_z  {dov};
  float   far_z   {80};
  GlCameraEuler cam (fov, dov, kWidth, kHeight, cam_pos, cam_dir, near_z, far_z);

  // Other stuff

  Buffer  buf (kWidth, kHeight, 0);
  GlText  text {win};
  Pos     mpos_prev {win.ReadMousePos()}; // to calc mouse pos between frames
  bool    cam_z_mode {false};

  do {
    timer.Start();
    win.Clear();

    auto    kbtn = win.ReadKeyboardBtn(BtnType::KB_DOWN);
    auto    mpos = win.ReadMousePos();
    auto    mbtn_pr = win.ReadMouseBtn(BtnType::MS_DOWN);
    auto    mbtn_rl = win.ReadMouseBtn(BtnType::MS_UP);
    if (mbtn_pr == Btn::LMB)
      cam_z_mode = true;
    if (mbtn_rl == Btn::LMB)
      cam_z_mode = false;

    Vector  obj_vel (0.0f, 0.0f, 0.0f);
    HandleCameraPosition(kbtn, cam.pos_);
    HandleCameraRotate(cam_z_mode, mpos, mpos_prev, cam.dir_);
    HandlePause(kbtn, win);
    HandleObject(kbtn, obj_vel, obj_rot, obj_scale);

    // Some hand transformation

    obj.world_pos_ += obj_vel;

    // Prepare transformation matrixes

    MatrixRotate      mx_rot {obj_rot, trig};
    MatrixTranslate   mx_trans {obj.world_pos_};
    MatrixPerspective mx_per {cam.dov_, cam.ar_};
    MatrixScale       mx_scale(obj_scale);
    
    // Prepare camera matrixes 

    Matrix<4,4>       mx_cam {};
    matrix::MakeIdentity(mx_cam);

    MatrixTranslate   mx_cam_trans  {cam.pos_ * (-1)};
    MatrixRotate      mx_cam_roty   {0.0f, -cam.dir_.y, 0.0f, trig};
    MatrixRotate      mx_cam_rotx   {-cam.dir_.x, 0.0f, 0.0f, trig};
    MatrixRotate      mx_cam_rotz   {0.0f, 0.0f, -cam.dir_.z, trig};
    mx_cam = matrix::Multiplie(mx_cam, mx_cam_trans); 
    mx_cam = matrix::Multiplie(mx_cam, mx_cam_roty); 
    mx_cam = matrix::Multiplie(mx_cam, mx_cam_rotx); 
    mx_cam = matrix::Multiplie(mx_cam, mx_cam_rotz); 

    // Prepare total matrix

    Matrix<4,4>       mx_total {};
    matrix::MakeIdentity(mx_total);

    // Rotate local coordinates

    obj.SetCoords(Coords::LOCAL);
    object::ApplyMatrix(mx_rot, obj);
    obj.CopyCoords(Coords::LOCAL, Coords::TRANS);

    // Transform trans coordinates

    mx_total = matrix::Multiplie(mx_total, mx_trans);
    mx_total = matrix::Multiplie(mx_total, mx_scale);
    obj.SetCoords(Coords::TRANS);
    object::ApplyMatrix(mx_total, obj);
 
    // Cull hidden surfaces

    object::ResetAttributes(obj);
    object::RemoveHiddenSurfaces(obj, cam);

    // Go from world coords to camera coords

    matrix::MakeIdentity(mx_total);
    mx_total = matrix::Multiplie(mx_total, mx_cam);
    object::ApplyMatrix(mx_total, obj);

    // Convert world_pos to camera_pos (from world into camera coords)  

    obj.camera_pos_ = matrix::Multiplie(obj.world_pos_, mx_cam);

    // Cull hidden objects
    
    object::Cull(obj, cam);

    // Go from camera coords to perspective

    matrix::MakeIdentity(mx_total);
    mx_total = matrix::Multiplie(mx_total, mx_per);
    object::ApplyMatrix(mx_total, obj);
    
    // Since after mx_per we have homogenous coords

    for (auto& vx : obj.GetCoords())
      vector::ConvertFromHomogeneous(vx);

    // Get screen coordinates

    MatrixViewport mx_view {cam.wov_, cam.scr_w_, cam.scr_h_};
    object::ApplyMatrix(mx_view, obj);

    // Draw triangles (stored in object)

    buf.Clear();
    draw::Object(obj, kWidth, kHeight, buf);
    buf.SendDataToFB();

    // Print fps ans other info
    
    PrintInfo(text, fps, obj.world_pos_, obj_rot, cam.pos_, cam.dir_);
    fps.Count();

    win.Render();
    timer.Wait();

  } while (!win.Closed());

  return 0;
}