// *************************************************************
// File:    array.cc
// Descr:   using triangles array demo
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

#include "helpers.h"

using namespace anshub;

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

  // Ethalon object

  auto obj = object::Make(
    "data/cube.ply", trig, 
    {1.0f, 1.0f, 1.0f},   // initial scale
    {0.0f, 0.0f, 0.0f},   // world pos
    {0.0f, 0.0f, 0.0f}    // initial rotate
  );
  Vector  obj_rot  {0.0f, 0.0f, 0.0f};
  
  // Create work objects, place randomly in space

  constexpr int kCubesCount {30};
  constexpr float kWorldSize {30};
  Objects cubes {kCubesCount, obj};

  // Here we change all references to vertexes inside triangles

  for (auto& cube : cubes)
  {
    for (auto& tri : cube.triangles_)
    {
      tri.v1_ = std::ref(cube.vxs_trans_[tri.f1_]);
      tri.v2_ = std::ref(cube.vxs_trans_[tri.f2_]);
      tri.v3_ = std::ref(cube.vxs_trans_[tri.f3_]);
      tri.c1_ = std::ref(cube.colors_trans_[tri.f1_]);
      tri.c2_ = std::ref(cube.colors_trans_[tri.f2_]);
      tri.c3_ = std::ref(cube.colors_trans_[tri.f3_]);
    }
  }

  for (auto& cube : cubes)
  {
    cube.world_pos_.x = rand_toolkit::get_rand(-kWorldSize, kWorldSize);
    cube.world_pos_.y = rand_toolkit::get_rand(-kWorldSize, kWorldSize);
    cube.world_pos_.z = rand_toolkit::get_rand(-kWorldSize, kWorldSize);
  }

  // Prepare vectors for rotating each cube in loop

  std::vector<Vector> cubes_rot {};
  cubes_rot.reserve(kCubesCount);
  
  for (int i = 0; i < kCubesCount; ++i)
  {
    cubes_rot.emplace_back(
      rand_toolkit::get_rand(-4.0f, 4.0f),
      rand_toolkit::get_rand(-4.0f, 4.0f),
      rand_toolkit::get_rand(-4.0f, 4.0f)
    );
  }

  // Camera

  float    dov     {2};
  float    fov     {60};
  Vector   cam_pos {0.0f, 0.0f, -kWorldSize};
  Vector   cam_dir {0.0f, 0.0f, 0.0f};
  float    near_z  {dov};
  float    far_z   {300};
  GlCamera cam (fov, dov, kWidth, kHeight, cam_pos, cam_dir, near_z, far_z);

  // Other stuff

  Buffer  buf (kWidth, kHeight, color::Black);
  GlText  text {win};

  do {
    timer.Start();
    win.Clear();

    // Handle input

    auto kbtn = win.ReadKeyboardBtn(BtnType::KB_DOWN);
    helpers::HandleCamMovement(kbtn, cam);

    // Rotate cubes

    objects::SetCoords(cubes, Coords::LOCAL);
    objects::Rotate(cubes, cubes_rot, trig);

    // Translate cubes to world

    objects::CopyCoords(cubes, Coords::LOCAL, Coords::TRANS);
    objects::SetCoords(cubes, Coords::TRANS);
    for (auto& cube : cubes)
      object::Translate(cube, cube.world_pos_);

    // Cull hidden surfaces

    objects::ResetAttributes(cubes);
    objects::Cull(cubes, cam);
    objects::RemoveHiddenSurfaces(cubes, cam);
    
    // Make triangles from objects

    auto tri_arr = triangles::MakeContainer();
    triangles::AddFromObjects(cubes, tri_arr);
    triangles::SortZ(tri_arr);

    // Finally

    objects::World2Camera(cubes, cam);
    objects::Camera2Persp(cubes, cam);
    objects::Homogenous2Normal(cubes);
    objects::Persp2Screen(cubes, cam);

    // Draw triangles

    buf.Clear();
    draw::SolidTriangles(tri_arr, buf);
    buf.SendDataToFB();

    win.Render();
    timer.Wait();    

  } while (!win.Closed());

  return 0;
}