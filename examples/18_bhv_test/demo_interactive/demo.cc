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

#include "lib/draw/gl_bvh.h"
#include "lib/draw/gl_object.h"
#include "lib/draw/gl_render_ctx.h"
#include "lib/draw/gl_draw.h"
#include "lib/draw/gl_text.h"
#include "lib/draw/gl_lights.cc"
#include "lib/draw/gl_coords.h"
#include "lib/draw/gl_z_buffer.h"
#include "lib/draw/cameras/gl_camera.h"

#include "../../helpers.h"

using namespace anshub;
using namespace helpers;

void PrintInfo(
  GlText& text, FpsCounter& fps, int objs_cnt,
  int pot_nodes_vis, int col_nodes_vis,
  int pot_col, int fac_col, Bvh& tree)
{
  using vector::operator<<;
  std::ostringstream oss {};
  
  oss << "FPS: " << fps.ReadPrev();
  text.PrintString(60, 110, oss.str().c_str());

  oss.str("");
  oss.clear();
  oss << "Objs in fact: " << objs_cnt << ", nodes checked: " << col_nodes_vis;
  text.PrintString(60, 90, oss.str().c_str());

  oss.str("");
  oss.clear();
  oss << "potential collided: " << pot_col << ", fact collided: " << fac_col;
  text.PrintString(60, 70, oss.str().c_str());

  oss.str("");
  oss.clear();
  oss << "Ptrs in tree: " << tree.ObjectsCount()
      << ", octs in tree: " << tree.OctantsCount();
  text.PrintString(60, 50, oss.str().c_str());
}


// Prints usage

void PrintUsage(std::ostream& oss)
{
  oss << "Usage: ./demo arg1..arg5 \n"
      << "  [fname_path]    - string\n"
      << "  [world_size]    - float\n"
      << "  [objs_in_world] - int\n"
      << "  [max_scale]     - float\n"
      << "  [tree_depth]    - int\n";
}

// Prints error message and usage

void PrintError(const char* msg, std::ostream&)
{
  std::cerr << msg << '\n';
  PrintUsage(std::cerr);
}

// Stores data inputed from cmd

struct InputData
{
  InputData(int argc, const char** argv);

  const char* fname_;
  float world_size_;
  int objs_count_;
  float max_scale_;
  int tree_depth_;

}; // struct InputData

// Constructs InputData

InputData::InputData(int argc, const char** argv)
{
  if (argc != 6)
  {
    PrintUsage(std::cerr);
    throw std::runtime_error("Invalid arguments count");
  }
  fname_ = argv[1];
  world_size_ = atof(argv[2]);
  objs_count_ = atoi(argv[3]);
  max_scale_ = atof(argv[4]);
  tree_depth_ = atoi(argv[5]);
}

// Makes objects randomly using given width world_size

auto MakeObjects(const TrigTable& trig, const char* fname, int cnt,
                 float world_size, float max_scale)
{
  auto obj = object::Make(            // creates object and make cnt of objects
    fname, trig, 
    {1.0f, 1.0f, 1.0f},   // initial scale
    {0.0f, 0.0f, 0.0f},   // world pos
    {0.0f, 0.0f, 0.0f}    // initial rotate
  );
  V_GlObject objs (cnt, obj);

  for (auto& obj : objs)              // randomly place and scale all objects 
  {

    // Make random world pos

    float rad = world_size / 2;
    obj.world_pos_.x = rand_toolkit::get_rand(-rad, rad);
    obj.world_pos_.y = rand_toolkit::get_rand(-rad, rad);
    obj.world_pos_.z = rand_toolkit::get_rand(-rad, rad);
    
    // Make random colors to the cubes faces

    for (auto& vx : obj.vxs_local_)
      vx.color_ = {
        rand_toolkit::get_rand(0.0f, 255.0f),
        rand_toolkit::get_rand(0.0f, 255.0f),
        rand_toolkit::get_rand(0.0f, 255.0f)
      };
    for (auto& face : obj.faces_)
      face.color_ = obj.vxs_local_[face[0]].color_;

    // Make random scaling

    float sfactor  = rand_toolkit::get_rand(1.0f, max_scale);
    obj.SetCoords(Coords::LOCAL);
    object::Scale(obj, {sfactor, sfactor, sfactor});
    obj.sphere_rad_ = object::FindFarthestCoordinate(obj);
    obj.SetCoords(Coords::TRANS);
  }
  return objs;
}

auto MakeCamera(float fov, float dov, int width, int height,
                const Vector& cam_pos, const Vector& cam_dir,
                float near_z, float far_z, const TrigTable& trig)
{
  CameraMan camman {
    fov, dov, width, height, cam_pos, cam_dir, near_z, far_z, trig
  };
  camman.SetButton(CamAction::STRAFE_LEFT, KbdBtn::A);
  camman.SetButton(CamAction::STRAFE_RIGHT, KbdBtn::D);
  camman.SetButton(CamAction::MOVE_FORWARD, KbdBtn::W);
  camman.SetButton(CamAction::MOVE_BACKWARD, KbdBtn::S);
  camman.SetButton(CamAction::MOVE_UP, KbdBtn::R);
  camman.SetButton(CamAction::MOVE_DOWN, KbdBtn::F);
  camman.SetButton(CamAction::ZOOM_IN, KbdBtn::NUM9);
  camman.SetButton(CamAction::ZOOM_OUT, KbdBtn::NUM0);
  camman.SetButton(CamAction::WIRED, KbdBtn::T, 20);
  camman.SetButton(CamAction::SWITCH_TYPE, KbdBtn::ENTER, 20);
  camman.SetButton(CamAction::SPEED_UP, KbdBtn::LSHIFT);
  camman.SetState(CamState::FLY_MODE, true);
  camman.SetValue(CamValue::MOUSE_SENSITIVE, 0.3f);

  // Set up camera's bounding directions

  auto& curr_cam = camman.GetCurrentCamera();
  curr_cam.SetDirection(GlCamera::YAW, 1.0f, 6.0f, -360.0f, 360.0f, false);  
  curr_cam.SetDirection(GlCamera::PITCH, 1.0f, 6.0f, -360.0f, 360.0f, false);  

  // Set up camera's dynamics

  Dynamics dyn {0.005f, 0.8f, -0.1f, 100.0f};
  camman.SetDynamics(std::move(dyn));

  return camman;
}

int DetectPotentialCollisions(Bvh& tree, const GlObject& main)
{
  auto potential = tree.FindPotential(main);
  for (auto* obj : potential)
  {
    for (auto& vx : obj->vxs_local_)
      vx.color_ = {
        rand_toolkit::get_rand(0.0f, 255.0f),
        rand_toolkit::get_rand(0.0f, 255.0f),
        rand_toolkit::get_rand(0.0f, 255.0f)
      };
    for (auto& face : obj->faces_)
      face.color_ = obj->vxs_local_[face[0]].color_;
  }
  return potential.size();
}

int DetectCurrentCollisions(Bvh& tree, const GlObject& main)
{
  auto collisions = tree.FindCollision(main);
  for (const auto& obj : collisions)
  {
      for (auto& vx : obj->vxs_local_)
      vx.color_ = {0.0f, 255.0f, 0.0f};
    for (auto& face : obj->faces_)
      face.color_ = obj->vxs_local_[face[0]].color_;
  }
  return collisions.size();
}

int main(int argc, const char** argv)
{
  // Process file name

  InputData args(argc, argv);

  // Math processor

  TrigTable trig {};
  rand_toolkit::start_rand();

  // Timers

  FpsCounter fps {};
  constexpr int kFpsWait = 1000;
  Timer timer (kFpsWait);

  // Window

  constexpr int kWidth = 800;
  constexpr int kHeight = 600;
  
  auto pos  = io_helpers::GetXYToMiddle(kWidth, kHeight); 
  GlWindow win (pos.x, pos.y, kWidth, kHeight, "Camera"); 
  auto mode = io_helpers::FindVideoMode(kWidth, kHeight);

  // Camera

  float    dov     {1.0f};
  float    fov     {75.0f};
  Vector   cam_pos {3.0f, 3.45f, 3.65f};
  Vector   cam_dir {27.0f, -31.0f, 0.0f};
  float    near_z  {1.5f};
  float    far_z   {500};
  
  auto camman = MakeCamera(
    fov, dov, kWidth, kHeight, cam_pos, cam_dir, near_z, far_z, trig);

  // Prepare lights sources
 
  Lights lights {};
  lights.AddAmbient(color::fWhite, 0.3f);
  lights.AddInfinite(color::fWhite, 0.7f, {-1.0f, -2.0f, -0.9f});

  // Create render context

  GlText text {win};
  RenderContext render_ctx(kWidth, kHeight, color::Black);
  render_ctx.is_zbuf_  = true;
  render_ctx.is_wired_ = false;
  render_ctx.is_alpha_ = false;
  render_ctx.is_bifiltering_ = false;
  render_ctx.is_mipmapping_  = false;
  render_ctx.clarity_  = camman.GetCurrentCamera().z_far_;

  // Make world objects and choose back as main object

  auto objs = MakeObjects(
    trig, args.fname_, args.objs_count_, args.world_size_, args.max_scale_);

  // Create bvh-tree and insert objects there

  Bvh tree (args.tree_depth_, args.world_size_);
  for (auto& obj : objs)
    tree.Insert(obj);

  // Create main object sticked to the camera

  auto main = object::Make(
    args.fname_, trig, 
    {3.5f, 3.5f, 3.5f},   // initial scale
    {0.0f, 0.0f, 0.0f},   // world pos
    {0.0f, 0.0f, 0.0f}    // initial rotate
  );

  // Make triangles arrays

  auto tris_base = triangles::MakeBaseContainer(0);
  auto tris_ptrs = triangles::MakePtrsContainer(0);

  do {
    timer.Start();
    win.Clear();

    // Handle input

    auto& cam = camman.GetCurrentCamera();
    camman.ProcessInput(win);

    auto pot_collided  = DetectPotentialCollisions(tree, main);
    auto pot_nodes_vis = tree.LastNodesVisits();
    auto fac_collided  = DetectCurrentCollisions(tree, main);
    auto col_nodes_vis = tree.LastNodesVisits();

    render_ctx.is_wired_ = camman.GetState(CamState::WIRED_MODE);    

    auto kbtn = win.ReadKeyboardBtn(BtnType::KB_DOWN);
    helpers::HandlePause(kbtn, win);
    helpers::HandleFullscreen(kbtn, mode, win);

    if (cam.type_ == CamType::UVN)
      camman.GetCamera(CamType::Uvn::type).LookAt({0.0f, 0.0f, 0.0f});

    // Process main object

    main.SetCoords(Coords::TRANS);
    main.CopyCoords(Coords::LOCAL, Coords::TRANS);
    main.world_pos_ = cam.vrp_;
    object::Translate(main, main.world_pos_);

    object::ResetAttributes(main);
    object::RemoveHiddenSurfaces(main, cam);
    object::ComputeFaceNormals(main);
    object::ComputeVertexNormalsV2(main);
    
    // Prepare triangles

    tris_base.resize(0);
    tris_ptrs.resize(0);
    
    // Process other objects

    for (auto& obj : objs)
    {
      obj.CopyCoords(Coords::LOCAL, Coords::TRANS);
      obj.SetCoords(Coords::TRANS);
      object::ResetAttributes(obj);

      object::Translate(obj, obj.world_pos_);    
      object::CullZ(obj, cam, trig);
      object::CullX(obj, cam, trig);
      object::CullY(obj, cam, trig);

      object::RemoveHiddenSurfaces(obj, cam);
      object::ComputeFaceNormals(obj, true);
      if (obj.active_)
        triangles::AddFromObject(obj, tris_base);
    }

    // Make triangles

    triangles::AddFromObject(main, tris_base);
    triangles::World2Camera(tris_base, cam, trig);
    
    // Light triangles in world coordinates
    
    triangles::CullAndClip(tris_base, cam);   // !!
    triangles::ComputeNormals(tris_base, true);
    light::World2Camera(lights, cam, trig);
    light::Triangles(tris_base, lights);
    light::Reset(lights);

    // !! todo: if move here, lighting of zclipped ok, To fix
    
    // Finish

    triangles::MakePointers(tris_base, tris_ptrs);
    triangles::SortZAvg(tris_ptrs);

    // Finally

    triangles::Camera2Persp(tris_base, cam);
    triangles::Persp2Screen(tris_base, cam);

    // Draw triangles

    render_ctx.is_wired_ = camman.GetState(CamState::WIRED_MODE);
    render::Context(tris_ptrs, render_ctx);

    // Print fps and other info
    
    PrintInfo(text, fps, objs.size(), pot_nodes_vis, col_nodes_vis,
              pot_collided, fac_collided, tree);
    fps.Count();
    
    win.Render();
    timer.Wait();

  } while (!win.Closed());

  return 0;
}