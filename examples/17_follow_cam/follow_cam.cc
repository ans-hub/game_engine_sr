// *************************************************************
// File:    follow_cam.cc
// Descr:   test follow camera
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <iomanip> 

#include "lib/window/gl_window.h"
#include "lib/window/helpers.h"

#include "lib/draw/gl_render_ctx.h"
#include "lib/draw/gl_object.h"
#include "lib/draw/gl_draw.h"
#include "lib/draw/gl_triangle.h"
#include "lib/draw/fx_colors.h"
#include "lib/draw/gl_lights.h"
#include "lib/draw/gl_coords.h"
#include "lib/draw/gl_z_buffer.h"
#include "lib/draw/gl_debug_draw.h"

#include "lib/extras/skybox.h"
#include "lib/extras/player.h"
#include "lib/extras/cameraman.h"
#include "lib/extras/terrain.h"

#include "lib/system/timer.h"
#include "lib/system/fps_counter.h"
#include "lib/system/rand_toolkit.h"

#include "lib/math/trig.h"

#include "../helpers.h"

using namespace anshub;
using namespace helpers;
using vector::operator<<;

int main(int argc, const char** argv)
{
  if (argc != 2)
  {
    std::cerr << "You must input config file name" << '\n';
    return false;
  }

  // Math processor
  
  TrigTable trig {};
  rand_toolkit::start_rand();

  // Timers

  FpsCounter fps {};
  Timer timer (1000);

  // Constants

  const int kWinWidth {800};
  const int kWinHeight {600};
  const bool kDebugShow {1};  

  // Window
  
  auto pos  = io_helpers::GetXYToMiddle(kWinWidth, kWinHeight); 
  auto mode = io_helpers::FindVideoMode(kWinWidth, kWinHeight);
  GlWindow win (pos.x, pos.y, kWinWidth, kWinHeight, "Follow camera");
  
  // Create terrain

  Terrain terrain (
    "../00_data/terrains/new_test_hm.bmp", "../00_data/terrains/new_test_tx.bmp",
    2.0f, 9, Shading::FLAT
  );
  terrain.SetDetalization({50.0f, 100.0f, 150.0f});
  auto& terrain_chunks = terrain.GetChunks();

  // Camera

  float    dov     {1.0f};
  float    fov     {75.0f};
  float    near_z  {0.1f};
  float    far_z   {200.0f};
  Vector   cam_pos {0.0f, 0.0f, 0.0f};
  Vector   cam_dir {0.0f, 0.0f, 1.0f};

  CameraMan camman {
    fov, dov, kWinWidth, kWinHeight, cam_pos, cam_dir, near_z, far_z, trig};
  camman.SetButton(CamAction::STRAFE_LEFT, KbdBtn::A);
  camman.SetButton(CamAction::STRAFE_RIGHT, KbdBtn::D);
  camman.SetButton(CamAction::MOVE_FORWARD, KbdBtn::W);
  camman.SetButton(CamAction::MOVE_BACKWARD, KbdBtn::S);
  camman.SetButton(CamAction::MOVE_UP, KbdBtn::R);
  camman.SetButton(CamAction::MOVE_DOWN, KbdBtn::F);
  camman.SetButton(CamAction::ZOOM_IN, KbdBtn::NUM9);
  camman.SetButton(CamAction::ZOOM_OUT, KbdBtn::NUM0);
  camman.SetButton(CamAction::LOOK_UP, KbdBtn::NUM1);
  camman.SetButton(CamAction::LOOK_DOWN, KbdBtn::NUM2);
  camman.SetButton(CamAction::WIRED, KbdBtn::T, 20);
  camman.SetButton(CamAction::SWITCH_TYPE, KbdBtn::ENTER, 20);
  camman.SetButton(CamAction::SPEED_UP, KbdBtn::LSHIFT);

  camman.SetState(CamState::FLY_MODE, true);
  camman.SetState(CamState::ON_GROUND, false);

  camman.SetValue(CamValue::OPERATOR_HEIGHT, 5.0f);
  camman.SetValue(CamValue::SPEED_UP, 2.0f);

  Dynamics dyn_camera {0.01f, 0.8f, -0.01f, 10.0f};
  camman.SetDynamics(std::move(dyn_camera));  

  // Create observable model

  const char*  kFname     {argv[1]};
  GlObject model (kFname, {0.0f, 0.0f, 0.0f});
  object::Rotate(model, {-90.0f, 0.0f, 0.0f}, trig);

  // Create player entity

  const float  kPlayerHeight {5.0f};
  const Vector kPlayerDir    {0.0f, 0.0f, 0.0f};
  const Vector kPlayerPos    {0.0f, 70.0f, 10.0f};
  Player obj (std::move(model), kPlayerHeight, kPlayerPos, kPlayerDir, trig);

  // Set player controls

  obj.SetButton(ObjAction::TURN_LEFT, KbdBtn::LEFT);
  obj.SetButton(ObjAction::TURN_RIGHT, KbdBtn::RIGHT);
  obj.SetButton(ObjAction::LOOK_UP, KbdBtn::Z);
  obj.SetButton(ObjAction::LOOK_DOWN, KbdBtn::X);
  obj.SetButton(ObjAction::ROLL_LEFT, KbdBtn::C);
  obj.SetButton(ObjAction::ROLL_RIGHT, KbdBtn::V);
  obj.SetButton(ObjAction::MOVE_FORWARD, KbdBtn::UP);
  obj.SetButton(ObjAction::MOVE_BACKWARD, KbdBtn::DOWN);
  obj.SetButton(ObjAction::MOVE_UP, KbdBtn::B);
  obj.SetButton(ObjAction::MOVE_DOWN, KbdBtn::N);
  obj.SetButton(ObjAction::SPEED_UP, KbdBtn::LCTRL);

  obj.SetState(ObjState::FLY_MODE, true);
  obj.SetState(ObjState::ON_GROUND, false);
  obj.SetValue(ObjValue::SPEED_UP, 2.0f);

  obj.SetDirection(Player::PITCH, 1.0f, 2.0f, 0.0f, 0.0f, false);
  obj.SetDirection(Player::ROLL, 1.0f, 2.0f, 0.0f, 0.0f, false);
  obj.SetDirection(Player::YAW, 1.0f, 2.0f, 0.0f, 0.0f, false);
  Dynamics dyn_player {0.01f, 0.8f, -0.01f, 10.0f};
  obj.SetDynamics(std::move(dyn_player));

  // Set initial camera position (by using orient_z vector)

  const Vector kDirOffset {26.5f, 0.0f, 0.0f};
  const Vector kVrpOffset {0.0f, 2.5f, -10.0f};

  camman.UseCamera(CamType::Follow::type);
  auto& follow_cam = camman.GetCamera(CamType::Follow::type);
  follow_cam.SetDirection(GlCamera::PITCH, 1.2f, 2.0f, 0.0f, 0.0f, false);
  follow_cam.SetDirection(GlCamera::ROLL, 0.8f, 2.0f, 0.0f, 0.0f, false);
  follow_cam.SetDirection(GlCamera::YAW, 1.0f, 2.0f, 0.0f, 0.0f, false);
  follow_cam.FollowFor(obj, kVrpOffset, kDirOffset);

  // Create render context

  RenderContext render_ctx(kWinWidth, kWinHeight, color::Black);
  render_ctx.is_zbuf_  = true;
  render_ctx.is_wired_ = false;
  render_ctx.is_alpha_ = false;
  render_ctx.is_bifiltering_ = false;
  render_ctx.is_mipmapping_ = false;
  render_ctx.clarity_  = 10.0f;
  
  // Make triangles arrays

  auto tris_base = triangles::MakeBaseContainer(0);
  auto tris_ptrs = triangles::MakePtrsContainer(0);
  auto tris_sky  = triangles::MakeBaseContainer(0);

  // Prepare lights sources

  Lights      lights_all {};
  ColorTable  color_table {};

  auto   color   = color::fWhite;
  auto   intense = 0.4f;
  Vector lpos {};
  Vector ldir {};

  lights_all.AddAmbient(color, intense);

  color   = color::fWhite;
  intense = 0.8f;
  ldir    = {0.0f, -1.0f, -1.0f};

  if (intense)
    lights_all.AddInfinite(color, intense, ldir);
  
  // Main loop

  do {
    timer.Start();
    win.Clear();

    auto& cam = camman.GetCurrentCamera();

    // Process system input

    auto kbtn = win.ReadKeyboardBtn(BtnType::KB_DOWN);
    helpers::HandlePause(kbtn, win);
    helpers::HandleFullscreen(kbtn, mode, win);

    // Process gameplay input 

    camman.ProcessInput(win);
    obj.ProcessInput(win);
    
    obj.ProcessPlayerOrientation();
    obj.ProcessPlayerRotating();

    follow_cam.FollowFor(obj);

    object::Translate(obj, obj.world_pos_);
    object::ResetAttributes(obj);
    object::ComputeFaceNormals(obj, true);
    auto hidden = object::RemoveHiddenSurfaces(obj, cam);

       // Cull terrain chunks by world pos

    int obj_culled {};
    for (auto& chunk : terrain_chunks)
    {
      object::ResetAttributes(chunk);
      obj_culled += object::CullZ(chunk, cam, trig);
      obj_culled += object::CullX(chunk, cam, trig);
      obj_culled += object::CullY(chunk, cam, trig);
    }

    // Change terrain detalization

    terrain.ProcessDetalization(cam.vrp_);
    
    for (auto& chunk : terrain_chunks)
    {
      if (!chunk.active_)
        continue;
      chunk.CopyCoords(Coords::LOCAL, Coords::TRANS);
      chunk.SetCoords(Coords::TRANS);

      object::ComputeFaceNormals(chunk, true);
      hidden += object::RemoveHiddenSurfaces(chunk, cam);
      object::VerticesNormals2Camera(chunk, cam, trig);
    }

    // Make triangles

    tris_base.resize(0);
    tris_ptrs.resize(0);
    tris_sky.resize(0);

    for (auto& chunk : terrain_chunks)
    {
      if (chunk.active_)
        triangles::AddFromObject(chunk, tris_base);
    }
    triangles::AddFromObject(obj, tris_base);
    triangles::World2Camera(tris_base, cam, trig);
    auto tri_culled = triangles::CullAndClip(tris_base, cam);
    
    // Light triangles in world coordinates

    triangles::ComputeNormals(tris_base);
    light::World2Camera(lights_all, cam, trig);
    light::Triangles(tris_base, lights_all);
    light::Reset(lights_all);

    // Triangles merging

    triangles::MakePointers(tris_base, tris_ptrs);
    triangles::SortZAvg(tris_ptrs);

    triangles::Camera2Persp(tris_base, cam);
    triangles::Persp2Screen(tris_base, cam);

    // Draw triangles

    render_ctx.is_wired_ = camman.GetState(CamState::WIRED_MODE);
    render::Context(tris_ptrs, render_ctx);
    
    // Finish frame rendering

    fps.Count();
    win.Render();
    timer.Wait();

    if (kDebugShow && fps.Ready())
    {
      std::cerr << "Frames per second: " << fps.ReadPrev() << '\n';
      std::cerr << "Camera position: " << cam.vrp_ << '\n';
      std::cerr << "Camera direction: " << cam.dir_ << '\n';
      std::cerr << "Hidden surfaces: " << hidden << '\n';
      std::cerr << "Triangles total: " << tris_base.size() << '\n';
      std::cerr << "Triangles culled: " << tri_culled << '\n';   
      std::cerr << '\n';
    }

  } while (!win.Closed());

  return 0;
}