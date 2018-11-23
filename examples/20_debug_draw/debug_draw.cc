// *************************************************************
// File:    debug_draw.cc
// Descr:   debug draw example
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
#include "lib/math/matrix.h"
#include "lib/math/matrices/mx_rotate_eul.h"
#include "lib/math/segment.h"
#include "lib/math/trig.h"

#include "lib/render/gl_render_ctx.h"
#include "lib/render/gl_debug_draw.h"
#include "lib/render/gl_draw.h"
#include "lib/render/gl_text.h"
#include "lib/render/gl_lights.cc"
#include "lib/render/gl_coords.h"
#include "lib/render/gl_object.h"
#include "lib/render/gl_z_buffer.h"
#include "lib/render/cameras/gl_camera.h"

#include "lib/extras/cameraman.h"

#include "../helpers.h"

using namespace anshub;
using namespace helpers;

int main(int argc, const char** argv)
{
  const char* obj_fname {};

  if (argc != 2) {
    std::cerr << "Incorrect file names\n";
    return 1;
  }
  else {
    obj_fname = argv[1];
  }
   
  TrigTable trig {};
  rand_toolkit::start_rand();

  FpsCounter fps {};
  constexpr int kFpsWait = 1000;
  Timer timer (kFpsWait);

  constexpr int kWidth = 800;
  constexpr int kHeight = 600;

  auto pos  = io_helpers::GetXYToMiddle(kWidth, kHeight); 
  GlWindow win (pos.x, pos.y, kWidth, kHeight, "Camera"); 
  auto mode = io_helpers::FindVideoMode(kWidth, kHeight);

  GlObject obj {obj_fname, {0.0f, 0.0f, 0.0f}};
  object::Scale(obj, {5.f, 5.f, 5.f});

  for (auto& vx : obj.vxs_local_)
    vx.color_.a_ = 0.1f;

  float    dov     {1.f};
  float    fov     {75.f};
  Vector   cam_pos {0.f, 0.f, -5.f};
  Vector   cam_dir {0.f, 0.f, 1.0f};
  float    near_z  {0.1f};
  float    far_z   {500.f};

  CameraMan camman {
    fov, dov, kWidth, kHeight, cam_pos, cam_dir, near_z, far_z, trig
  };
  camman.SetButton(CamAction::STRAFE_LEFT, KbdBtn::A);
  camman.SetButton(CamAction::STRAFE_RIGHT, KbdBtn::D);
  camman.SetButton(CamAction::MOVE_FORWARD, KbdBtn::W);
  camman.SetButton(CamAction::MOVE_BACKWARD, KbdBtn::S);
  camman.SetButton(CamAction::MOVE_UP, KbdBtn::R);
  camman.SetButton(CamAction::MOVE_DOWN, KbdBtn::F);
  camman.SetButton(CamAction::JUMP, KbdBtn::SPACE);
  camman.SetButton(CamAction::ZOOM_IN, KbdBtn::NUM9);
  camman.SetButton(CamAction::ZOOM_OUT, KbdBtn::NUM0);
  camman.SetButton(CamAction::TURN_LEFT, KbdBtn::NUM1);
  camman.SetButton(CamAction::TURN_RIGHT, KbdBtn::NUM2);
  camman.SetButton(CamAction::LOOK_UP, KbdBtn::NUM3);
  camman.SetButton(CamAction::LOOK_DOWN, KbdBtn::NUM4);
  camman.SetButton(CamAction::ROLL_MODE, KbdBtn::L, 20);
  camman.SetButton(CamAction::WIRED, KbdBtn::T, 20);
  camman.SetButton(CamAction::BIFILTERING_MODE, KbdBtn::I, 20);
  camman.SetButton(CamAction::MIPMAP_MODE, KbdBtn::O, 20);
  camman.SetButton(CamAction::SWITCH_TYPE, KbdBtn::ENTER, 20);
  camman.SetButton(CamAction::SPEED_UP, KbdBtn::LSHIFT);

  camman.SetState(CamState::FLY_MODE, true);
  camman.SetState(CamState::BIFILTERING_MODE, true);
  camman.SetState(CamState::MIPMAP_MODE, true);

  camman.SetValue(CamValue::MOUSE_SENSITIVE, 0.3f);

  auto& curr_cam = camman.GetCurrentCamera();
  curr_cam.SetDirection(GlCamera::YAW, 1.0f, 6.0f, -360.0f, 360.0f, false);  
  curr_cam.SetDirection(GlCamera::PITCH, 1.0f, 6.0f, -360.0f, 360.0f, false);  

  Physics dyn {0.005f, 0.7f, -0.1f, 100.0f};
  camman.SetPhysics(std::move(dyn));

  Lights lights {};
  lights.AddAmbient(color::fWhite, 0.3f);
  lights.AddInfinite(color::fWhite, 0.7f, {-1.0f, -2.0f, -0.9f});
  lights.AddPoint(color::fBlue, 0.6f, {0.0f, 0.0f, 10.0f}, {0.0f, 0.0f, -1.0f});

  RenderContext render_ctx(kWidth, kHeight, color::Black);
  render_ctx.is_zbuf_  = true;
  render_ctx.is_wired_ = false;
  render_ctx.is_alpha_ = true;
  render_ctx.is_bifiltering_ = true;
  render_ctx.is_mipmapping_  = true;
  render_ctx.mipmap_dist_ = 200.0f;
  render_ctx.clarity_ = camman.GetCurrentCamera().z_far_;
  render_ctx.cam_ = &camman.GetCurrentCamera();

  GlText text {win};
  Vector obj_rot {0.0f, 0.0f, 0.0f};

  auto tris_base = triangles::MakeBaseContainer(0);
  auto tris_ptrs = triangles::MakePtrsContainer(0);

  DebugContext debug {};
  debug.render_first_ = true;

  do {
    timer.Start();
    win.Clear();

    auto& cam = camman.GetCurrentCamera();
    render_ctx.cam_ = &cam;
    camman.ProcessInput(win);

    render_ctx.is_wired_ = camman.GetState(CamState::WIRED_MODE);    

    auto kbtn = win.ReadKeyboardBtn(BtnType::KB_DOWN);
    helpers::HandlePause(kbtn, win);
    helpers::HandleFullscreen(kbtn, mode, win);

    Vector  obj_vel    {0.0f, 0.0f, 0.0f};
    Vector  obj_scale  {1.0f, 1.0f, 1.0f};
    helpers::HandleObject(kbtn, obj_vel, obj_rot, obj_scale);

    obj.world_pos_ += obj_vel;

    render_ctx.is_mipmapping_ = camman.GetState(CamState::MIPMAP_MODE);
    render_ctx.is_bifiltering_ = camman.GetState(CamState::BIFILTERING_MODE);

    if (cam.type_ == CamType::UVN)
      camman.GetCamera(CamType::Uvn::type).LookAt(obj.world_pos_);

    obj.SetCoords(Coords::LOCAL);
    object::Rotate(obj, obj_rot, trig);
    obj.CopyCoords(Coords::LOCAL, Coords::TRANS);
    obj.SetCoords(Coords::TRANS);
    object::Translate(obj, obj.world_pos_);

    object::ResetAttributes(obj);
    auto hidden = 0;

    object::ComputeFaceNormals(obj);
    object::ComputeVertexNormalsV2(obj);
    light::Object(obj, lights);
    for (auto& vx : obj.GetCoords())
      debug.AddLine(vx.pos_, vx.pos_ + vx.normal_ * 2.f, color::fYellow);

    object::World2Camera(obj, cam, trig);
    tris_base.resize(0);
    tris_ptrs.resize(0);
    triangles::AddFromObject(obj, tris_base);
    auto culled = triangles::CullAndClip(tris_base, cam);
    triangles::MakePointers(tris_base, tris_ptrs);
    triangles::SortZAvg(tris_ptrs);
    
    triangles::Camera2Persp(tris_base, cam);
    triangles::Persp2Screen(tris_base, cam);

    Vector start {0.f, 0.f, 0.f};
    Vector fwd {0.f, 0.f, 1.f};
    Vector up {0.f, 1.f, 0.f};
    Vector right {1.f, 0.f, 0.f};
    debug.AddLine(start, start + up * 10.f, color::fWhite * 0.5f);
    debug.AddLine(start, start + up * -10.f, color::fWhite * 0.25f);
    debug.AddLine(start, start + fwd * 10.f, color::fWhite * 0.5f);
    debug.AddLine(start, start + fwd * -10.f, color::fWhite * 0.25f);
    debug.AddLine(start, start + right * 10.f, color::fWhite * 0.5f);
    debug.AddLine(start, start + right * -10.f, color::fWhite * 0.25f);

    // Debug draw axis of our lhs coordinate system from origin

    Matrix<4,4> tm {};
    float mp {1.f};
    tm.SetRow(0, fwd);
    tm.SetRow(1, up);
    tm.SetRow(2, right);
    tm.SetRow(3, start);
    debug.AddLine(start, start + tm.GetRow(0) * mp, color::fRed);
    debug.AddLine(start, start + tm.GetRow(1) * mp, color::fGreen);
    debug.AddLine(start, start + tm.GetRow(2) * mp, color::fBlue);

    // Make rotate matrix and rotate by it (as it lhs sys - pos angle rotates cw)

    Matrix<4,4> rotX = matrix::MakeRotateX(7.f);
    Matrix<4,4> rotY = matrix::MakeRotateY(7.f);
    Matrix<4,4> rot = rotY * rotX;

    // Rotate point

    Vector pt { 1.f, 1.f, 1.f};
    Vector dir {0.f, 0.f, 1.f};
    debug.AddLine(pt, pt + dir, color::fBlue);
    dir = rot % dir;
    debug.AddLine(pt, pt + dir, color::fBlue * 0.5f);

    // Rotate matrix 

    tm = tm % rot;
    rot = matrix::Transpose(rot);
    tm = tm % rot;
    debug.AddLine(start, start + tm.GetRow(0) * mp, color::fRed * 0.5f);
    debug.AddLine(start, start + tm.GetRow(1) * mp, color::fGreen * 0.5f);
    debug.AddLine(start, start + tm.GetRow(2) * mp, color::fBlue * 0.5f);
    
    // Expected result: as it lhs (pos z fwd, vector right) coord system,
    // positive angle rotates dir vector to positive angle

    // Draw normals
    
    // ...todo

    // Finalize

    render::Context(tris_ptrs, render_ctx, debug);
    fps.Count();
    win.Render();
    timer.Wait();

  } while (!win.Closed());

  return 0;
}