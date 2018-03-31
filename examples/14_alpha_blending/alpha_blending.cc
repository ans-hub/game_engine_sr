// *************************************************************
// File:    alpha_blending.cc
// Descr:   alpha blending example
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
#include "lib/draw/gl_render_ctx.h"
#include "lib/draw/gl_draw.h"
#include "lib/draw/gl_text.h"
#include "lib/draw/gl_lights.cc"
#include "lib/draw/gl_coords.h"
#include "lib/draw/gl_object.h"
#include "lib/draw/gl_z_buffer.h"
#include "lib/draw/gl_camera.h"
#include "lib/math/matrix_rotate_eul.h"
#include "lib/math/matrix_rotate_uvn.h"
#include "lib/math/matrix_persp.h"
#include "lib/math/matrix_trans.h"
#include "lib/math/matrix_view.h"
#include "lib/math/matrix_scale.h"
#include "lib/math/matrix_camera.h"

#include "../helpers.h"
#include "../camera_operator.h"

using namespace anshub;
using namespace helpers;

// Creates array of rectangles (w_cnt * h_cnt size) 

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
  // Hanle input filenames

  const char* obj1_fname {};
  const char* obj2_fname {};

  if (argc != 3) {
    std::cerr << "Incorrect file names\n";
    return 1;
  }
  else {
    obj1_fname = argv[1];
    obj2_fname = argv[2];
  }
   
  // Math processor
  
  TrigTable trig {};
  rand_toolkit::start_rand();

  // Timers

  FpsCounter fps {};
  constexpr int kFpsWait = 1000;
  Timer timer (kFpsWait);

  // Constants

  constexpr int kWidth = 800;
  constexpr int kHeight = 600;

  // Window

  auto pos  = io_helpers::GetXYToMiddle(kWidth, kHeight); 
  GlWindow win (pos.x, pos.y, kWidth, kHeight, "Camera"); 

  // Objects

  auto obj_1 = object::Make(
    obj1_fname, trig, 
    {2.0f, 2.0f, 2.0f},     // initial scale
    {0.0f, 0.0f, 10.0f},    // world pos
    {0.0f, 0.0f, 0.0f}      // initial rotate
  );

  // Make object 1 transparent
  
  if (!obj_1.textures_.empty())
  {
    for (auto& vx : obj_1.vxs_local_)
    {
      vx.color_ = FColor{color::Blue};
      vx.color_.a_ = 0.5f;
    }
  }
  else
  {
    for (auto& vx : obj_1.vxs_local_)
      vx.color_.a_ = 0.5f;
  }

  auto obj_2 = object::Make(
    obj2_fname, trig, 
    {2.0f, 2.0f, 2.0f},     // initial scale
    {2.0f, 5.0f, 30.0f},    // world pos
    {0.0f, 0.0f, 0.0f}      // initial rotate
  );
  auto obj_3 (obj_2);
  obj_3.world_pos_.z = 0.0f;
  obj_3.world_pos_.y -= 5.0f;
  V_GlObject objs {obj_2, obj_3};

  // Camera

  float    dov     {1.0f};
  float    fov     {75.0f};
  Vector   cam_pos {0.0f, 0.0f, -10.0f};
  Vector   cam_dir {0.0f, 0.0f, 0.0f};
  float    near_z  {dov};
  float    far_z   {500};

  CameraOperator cam {
    fov, dov, kWidth, kHeight, cam_pos, cam_dir, near_z, far_z
  };
  cam.SetPrevMousePos(win.ReadMousePos());
  cam.SetLeftButton(KbdBtn::A);
  cam.SetRightButton(KbdBtn::D);
  cam.SetForwardButton(KbdBtn::W);
  cam.SetBackwardButton(KbdBtn::S);
  cam.SetUpButton(KbdBtn::R);
  cam.SetDownButton(KbdBtn::F);
  cam.SetSpeedUpButton(KbdBtn::LSHIFT);
  cam.SetZoomInButton(KbdBtn::NUM9);
  cam.SetZoomOutButton(KbdBtn::NUM0);
  cam.SetSwitchRollButton(KbdBtn::L);
  cam.SetWiredModeButton(KbdBtn::T);
  cam.SetSwitchTypeButton(KbdBtn::ENTER);
  cam.SetMoveVelocity({0.0f, 0.0f, 0.2f});
  cam.SetFlyMode(true);

  // Prepare lights sources
 
  Lights lights {};
  FColor kWhite  {255.0f, 255.0f, 255.0f};
  FColor kBlack  {0.0f, 0.0f, 0.0f};
  FColor kYellow {255.0f, 255.0f, 0.0f};
  FColor kBlue   {0.0f, 0.0f, 255.0f};

  lights.ambient_.emplace_back(kWhite, 0.3f);
  lights.infinite_.emplace_back(kWhite, 0.7f, Vector{-1.0f, -2.0f, -0.9f});

  // Create render context

  RenderContext render_ctx(kWidth, kHeight, color::Black);
  render_ctx.is_zbuf_  = true;
  render_ctx.is_wired_ = false;
  render_ctx.is_alpha_ = true;
  render_ctx.clarity_  = 0.0f;

  GlText  text {win};
  Vector  obj_rot    {0.0f, 0.0f, 0.0f};

  // Make triangles arrays

  auto tris_base = triangles::MakeBaseContainer(0);
  auto tris_ptrs = triangles::MakePtrsContainer(0);

  do {
    timer.Start();
    win.Clear();

    // Handle input

    Vector  obj_vel    {0.0f, 0.0f, 0.0f};
    Vector  obj_scale  {1.0f, 1.0f, 1.0f};
    cam.ProcessInput(win);
    render_ctx.is_wired_ = cam.IsWired();    
    auto kbtn = win.ReadKeyboardBtn(BtnType::KB_DOWN);
    helpers::HandlePause(kbtn, win);
    helpers::HandleObject(kbtn, obj_vel, obj_rot, obj_scale);
    obj_1.world_pos_ += obj_vel;

    // Some hand transformation

    obj_1.SetCoords(Coords::LOCAL);
    object::Rotate(obj_1, obj_rot, trig);
    obj_1.CopyCoords(Coords::LOCAL, Coords::TRANS);
    obj_1.SetCoords(Coords::TRANS);
    object::Translate(obj_1, obj_1.world_pos_);

    objects::SetCoords(objs, Coords::LOCAL);
    objects::CopyCoords(objs, Coords::LOCAL, Coords::TRANS);
    objects::SetCoords(objs, Coords::TRANS);
    for (auto& obj : objs)
      object::Translate(obj, obj.world_pos_);

    // Culling

    object::ResetAttributes(obj_1);
    objects::ResetAttributes(objs);

    auto hidden = object::RemoveHiddenSurfaces(obj_1, cam);
    hidden += objects::RemoveHiddenSurfaces(objs, cam);

    // Light objects

    object::ComputeFaceNormals(obj_1);
    objects::ComputeFaceNormals(objs);
    object::ComputeVertexNormalsV2(obj_1);
    objects::ComputeVertexNormalsV2(objs);
    light::Object(obj_1, lights);
    light::Objects(objs, lights);

    // Camera routines (go to cam coords)

    MatrixCamera mx_cam {};
    if (cam.type_ == GlCamera::Type::EULER)
    {
      MatrixTranslate   mx_cam_trans  {cam.vrp_ * (-1)};
      MatrixRotateEul   mx_cam_rot    {cam.dir_ * (-1), trig};
      mx_cam = matrix::Multiplie(mx_cam, mx_cam_trans);
      mx_cam = matrix::Multiplie(mx_cam, mx_cam_rot);
    }
    else
    {
      cam.LookAt(obj_1.world_pos_);
      MatrixTranslate   mx_cam_trans  {cam.vrp_ * (-1)};
      MatrixRotateUvn   mx_cam_rot    {cam.u_, cam.v_, cam.n_};
      mx_cam = matrix::Multiplie(mx_cam_trans, mx_cam_rot);
      cam.dir_ = coords::RotationMatrix2Euler(mx_cam_rot);
    }

    object::ApplyMatrix(mx_cam, obj_1);
    objects::ApplyMatrix(mx_cam, objs);

    // Make triangles

    tris_base.resize(0);
    tris_ptrs.resize(0);
    triangles::AddFromObject(obj_1, tris_base);
    triangles::AddFromObjects(objs, tris_base);
    auto culled = triangles::CullAndClip(tris_base, cam);
    triangles::MakePointers(tris_base, tris_ptrs);
    triangles::SortZAvg(tris_ptrs);
    
    // Finally
    
    triangles::Camera2Persp(tris_base, cam);
    triangles::Persp2Screen(tris_base, cam);

    // Draw

    render::Context(tris_ptrs, render_ctx);
    
    // Print fps and other info
    
    PrintInfo(
      text, fps, obj_1.world_pos_, obj_rot, cam.vrp_, cam.dir_, culled, hidden
    );
    fps.Count();

    win.Render();
    timer.Wait();

  } while (!win.Closed());

  return 0;
}