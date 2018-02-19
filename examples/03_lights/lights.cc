// *************************************************************
// File:    obj_station
// Descr:   obj station demo
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#include <iostream>

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
#include "lib/draw/gl_coords.h"
#include "lib/draw/gl_object.h"
#include "lib/draw/gl_camera.h"
#include "lib/math/matrix_rotate_eul.h"
#include "lib/math/matrix_rotate_uvn.h"
#include "lib/math/matrix_persp.h"
#include "lib/math/matrix_trans.h"
#include "lib/math/matrix_view.h"
#include "lib/math/matrix_scale.h"
#include "lib/math/matrix_camera.h"

using namespace anshub;

// #include "helpers.h"

int main(int argc, const char** argv)
{
  // Math processor
  
  TrigTable trig {};
  rand_toolkit::start_rand();

  // Timers

  constexpr int kFpsWait = 1000;
  Timer timer (kFpsWait);

  // Window

  constexpr int kWidth = 800;
  constexpr int kHeight = 600;
  auto pos  = io_helpers::GetXYToMiddle(kWidth, kHeight); 
  GlWindow win (pos.x, pos.y, kWidth, kHeight, "Camera"); 

  // Object

  auto obj = object::Make(
    "data/tri_gourang.ply", trig, 
    {1.0f, 1.0f, 1.0f},   // initial scale
    {0.0f, 0.0f, 7.0f},   // world pos
    // {0.0f, 0.0f, 35.0f}   // initial rotate
    {0.0f, 0.0f, 2.0f}   // initial rotate
  );
  
  // Camera

  float    dov     {2};
  float    fov     {90};
  Vector   cam_pos {2.0f, -2.0f, 0.0f};
  Vector   cam_dir {0.0f, 0.0f, 0.0f};
  float    near_z  {dov};
  float    far_z   {500};
  GlCamera cam (fov, dov, kWidth, kHeight, cam_pos, cam_dir, near_z, far_z);

  // Other stuff

  Buffer  buf (kWidth, kHeight, color::Black);
  GlText  text {win};
  Pos     mpos_prev {win.ReadMousePos()}; // to calc mouse pos between frames

  do {
    timer.Start();
    win.Clear();

    Vector  obj_rot  {0.0f, 0.0f, 0.0f};
    auto kbtn = win.ReadKeyboardBtn(BtnType::KB_DOWN);
    if (kbtn == Btn::Z)
      obj_rot.z = -1.0f;
    else if (kbtn == Btn::X)
      obj_rot.z = +1.0f;
    if (kbtn == Btn::C)
      obj_rot.y = -1.0f;
    else if (kbtn == Btn::V)
      obj_rot.y = +1.0f;
    if (kbtn == Btn::B)
      obj_rot.x = -1.0f;
    else if (kbtn == Btn::N)
      obj_rot.x = +1.0f;
      
    // Prepare transformation matrixes for main object

    MatrixRotateEul   mx_rot {obj_rot, trig};
    MatrixTranslate   mx_trans {obj.world_pos_};
    MatrixPerspective mx_per {cam.dov_, cam.ar_};

    // Rotate local coordinates of main object

    obj.SetCoords(Coords::LOCAL);
    object::ApplyMatrix(mx_rot, obj);
    object::RefreshOrientation(obj, mx_rot);
    obj.CopyCoords(Coords::LOCAL, Coords::TRANS);

    // Transform trans coordinates of main object

    obj.SetCoords(Coords::TRANS);
    object::ApplyMatrix(mx_trans, obj);
    
    // Prepare camera`s matrixes (Euler or uvn) for all objects

    MatrixCamera      mx_cam {};
    if (cam.type_ == GlCamera::Type::EULER)
    {
      MatrixTranslate   mx_cam_trans  {cam.vrp_ * (-1)};
      MatrixRotateEul   mx_cam_rot    {cam.dir_ * (-1), trig};
      mx_cam = matrix::Multiplie(mx_cam, mx_cam_trans);
      mx_cam = matrix::Multiplie(mx_cam, mx_cam_rot);
    }
    else
    {
      cam.LookAt(obj.world_pos_);
      MatrixTranslate   mx_cam_trans  {cam.vrp_ * (-1)};
      MatrixRotateUvn   mx_cam_rot    {cam.u_, cam.v_, cam.n_};
      mx_cam = matrix::Multiplie(mx_cam_trans, mx_cam_rot);
      cam.dir_ = coords::RotationMatrix2Euler(mx_cam_rot);
    }

    // Cull hidden surfaces

    object::ResetAttributes(obj);
    object::Cull(obj, cam, mx_cam);
    object::RemoveHiddenSurfaces(obj, cam);
    
    // Go from world coords to camera, and the perspective coords

    Matrix<4,4> mx_total {};
    matrix::MakeIdentity(mx_total);
    mx_total = matrix::Multiplie(mx_total, mx_cam);
    mx_total = matrix::Multiplie(mx_total, mx_per);
    object::ApplyMatrix(mx_total, obj);

    // Since after mx_per we have homogenous coords

    coords::Homogenous2Normal(obj);

    // Get screen coordinates

    MatrixViewport mx_view {cam.wov_, cam.scr_w_, cam.scr_h_};
    object::ApplyMatrix(mx_view, obj);

    // Draw triangles (stored in object)

    buf.Clear();
    draw::SolidObject(obj, kWidth, kHeight, buf);
    buf.SendDataToFB();

    win.Render();
    timer.Wait();    

  } while (!win.Closed());

  return 0;
}