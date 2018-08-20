// *************************************************************
// File:    debug_draw.cc
// Descr:   debug draw example
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
#include "lib/math/matrix.h"
#include "lib/math/matrixes/mx_rotate_eul.h"
#include "lib/math/segment.h"
#include "lib/math/trig.h"

#include "lib/draw/gl_render_ctx.h"
#include "lib/draw/gl_debug_draw.h"
#include "lib/draw/gl_draw.h"
#include "lib/draw/gl_text.h"
#include "lib/draw/gl_lights.cc"
#include "lib/draw/gl_coords.h"
#include "lib/draw/gl_object.h"
#include "lib/draw/gl_z_buffer.h"
#include "lib/draw/cameras/gl_camera.h"

#include "lib/extras/cameraman.h"

#include "../helpers.h"

using namespace anshub;
using namespace helpers;

int main(int argc, const char** argv)
{
  // Hanle input filename

  const char* obj_fname {};

  if (argc != 2) {
    std::cerr << "Incorrect file names\n";
    return 1;
  }
  else {
    obj_fname = argv[1];
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
  auto mode = io_helpers::FindVideoMode(kWidth, kHeight);

  // Objects

  GlObject obj {obj_fname, {0.0f, 0.0f, 0.0f}};
  object::Scale(obj, {1.5f, 1.5f, 1.5f});

  // Camera

  float    dov     {1.0f};
  float    fov     {75.0f};
  Vector   cam_pos {-3.0f, 3.45f, 3.65f};
  Vector   cam_dir {27.0f, -31.0f, 0.0f};
  float    near_z  {1.5f};
  float    far_z   {500};

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

  Dynamics dyn {0.005f, 0.8f, -0.1f, 100.0f};
  camman.SetDynamics(std::move(dyn));

  // Prepare lights sources
 
  Lights lights {};
  lights.AddAmbient(color::fWhite, 0.3f);
  lights.AddInfinite(color::fWhite, 0.7f, {-1.0f, -2.0f, -0.9f});
  lights.AddPoint(color::fBlue, 0.6f, {0.0f, 0.0f, 10.0f}, {0.0f, 0.0f, -1.0f});

  // Create render context

  RenderContext render_ctx(kWidth, kHeight, color::Black);
  render_ctx.is_zbuf_  = true;
  render_ctx.is_wired_ = false;
  render_ctx.is_alpha_ = false;
  render_ctx.is_bifiltering_ = true;
  render_ctx.is_mipmapping_  = true;
  render_ctx.mipmap_dist_ = 200.0f;
  render_ctx.clarity_ = camman.GetCurrentCamera().z_far_;
  render_ctx.cam_ = &camman.GetCurrentCamera();

  GlText  text {win};
  Vector  obj_rot {0.0f, 0.0f, 0.0f};

  // Make triangles arrays

  auto tris_base = triangles::MakeBaseContainer(0);
  auto tris_ptrs = triangles::MakePtrsContainer(0);

  // Debug draw

  DebugContext debug {};

  do {
    timer.Start();
    win.Clear();

    // Handle input

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

    // Some hand transformation

    obj.SetCoords(Coords::LOCAL);
    object::Rotate(obj, obj_rot, trig);
    obj.CopyCoords(Coords::LOCAL, Coords::TRANS);
    obj.SetCoords(Coords::TRANS);
    object::Translate(obj, obj.world_pos_);

    // Culling

    object::ResetAttributes(obj);
    auto hidden = object::RemoveHiddenSurfaces(obj, cam);

    // Light objects

    object::ComputeFaceNormals(obj);
    object::ComputeVertexNormalsV2(obj);
    light::Object(obj, lights);

    // Make triangles

    object::World2Camera(obj, cam, trig);
    tris_base.resize(0);
    tris_ptrs.resize(0);
    triangles::AddFromObject(obj, tris_base);
    auto culled = triangles::CullAndClip(tris_base, cam);
    triangles::MakePointers(tris_base, tris_ptrs);
    triangles::SortZAvg(tris_ptrs);
    
    // Finally
    
    triangles::Camera2Persp(tris_base, cam);
    triangles::Persp2Screen(tris_base, cam);

    // Draw axis (debug)

    Vector start {0.f, 0.f, 0.f};
    Matrix<4,4> tm {};
    tm.SetRow(0, Vector{0.f, 0.f, 1.f});
    tm.SetRow(1, Vector{0.f, 1.f, 0.f});
    tm.SetRow(2, Vector{1.f, 0.f, 0.f});
    tm.SetRow(3, Vector{0.f, 0.f, 0.f});
    debug.AddLine(start, start + tm.GetRow(0) * 3.f, color::fRed);
    debug.AddLine(start, start + tm.GetRow(1) * 3.f, color::fGreen);
    debug.AddLine(start, start + tm.GetRow(2) * 3.f, color::fBlue);

    // Rotate axis (debug)

    MatrixRotateEul rot {3.f, 3.f, 0.f, trig};
    tm = matrix::Multiplie(tm, rot);
    debug.AddLine(start, start + tm.GetRow(0) * 3.f, color::fRed);
    debug.AddLine(start, start + tm.GetRow(1) * 3.f, color::fGreen);
    debug.AddLine(start, start + tm.GetRow(2) * 3.f, color::fBlue);

    // Finalize

    render::Context(tris_ptrs, render_ctx, debug);
    fps.Count();
    win.Render();
    timer.Wait();

  } while (!win.Closed());

  return 0;
}