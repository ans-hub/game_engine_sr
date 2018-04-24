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

using namespace anshub;
using namespace helpers;

void PrintInfo(
  GlText& text, FpsCounter& fps, 
  const Vector& obj_pos, const Vector& obj_rot, 
  const Vector& cam_pos, const Vector& cam_rot,
  int nfo_culled, int nfo_hidden)
{
  using vector::operator<<;
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
  
  if (obj_1.textures_.empty())
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
  V_GlObject objs {};
  obj_2.world_pos_.z = 0.0f;
  objs.push_back(obj_2);
  obj_2.world_pos_.z = 30.0f;
  obj_2.world_pos_.y -= 5.0f;
  objs.push_back(obj_2);

  // Camera

  float    dov     {1.0f};
  float    fov     {75.0f};
  Vector   cam_pos {0.0f, 0.0f, -10.0f};
  Vector   cam_dir {0.0f, 0.0f, 0.0f};
  float    near_z  {dov};
  float    far_z   {500};

  auto camman = MakeCameraman(
    fov, dov, kWidth, kHeight, cam_pos, cam_dir, near_z, far_z, trig);

  camman.SetValue(CamValue::MOUSE_SENSITIVE, 1.0f);

  Dynamics dyn {0.01f, 0.85f, -0.1f, 100.0f};
  camman.SetDynamics(std::move(dyn));

  // Prepare lights sources
 
  Lights lights {};
  lights.AddAmbient(color::fWhite, 0.3f);
  lights.AddInfinite(color::fWhite, 0.7f, {-1.0f, -2.0f, -0.9f});
  lights.AddPoint(color::fBlue, 0.6f, {0.0f, 3.0f, 5.0f}, {0.0f, 0.0f, -1.0f});

  // Create render context

  RenderContext render_ctx(kWidth, kHeight, color::Black);
  render_ctx.is_zbuf_  = true;
  render_ctx.is_wired_ = false;
  render_ctx.is_alpha_ = true;
  render_ctx.clarity_  = far_z;

  GlText  text {win};
  Vector  obj_rot    {0.0f, 0.0f, 0.0f};

  // Make triangles arrays

  auto tris_base = triangles::MakeBaseContainer(0);
  auto tris_ptrs = triangles::MakePtrsContainer(0);

  do {
    timer.Start();
    win.Clear();

    // Handle input

    auto& cam = camman.GetCurrentCamera();    
    camman.ProcessInput(win);
    render_ctx.is_wired_ = camman.GetState(CamState::WIRED_MODE);    

    auto kbtn = win.ReadKeyboardBtn(BtnType::KB_DOWN);
    helpers::HandlePause(kbtn, win);

    Vector  obj_vel    {0.0f, 0.0f, 0.0f};
    Vector  obj_scale  {1.0f, 1.0f, 1.0f};
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

    // Go to camera coords

    if (cam.type_ == CamType::UVN)
      camman.GetCamera(CamType::Uvn::type).LookAt(obj_1.world_pos_);

    object::World2Camera(obj_1, cam, trig);
    objects::World2Camera(objs, cam, trig);

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