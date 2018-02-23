// *************************************************************
// File:    objects.cc
// Descr:   using objects functions demo
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
#include "lib/draw/gl_lights.h"
#include "lib/draw/gl_object.h"
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

int main(int argc, const char** argv)
{
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
  GlObjects cubes {kCubesCount, obj};

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
      rand_toolkit::get_rand(-1.0f, 1.0f),
      rand_toolkit::get_rand(-1.0f, 1.0f),
      rand_toolkit::get_rand(-1.0f, 1.0f)
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

  // Prepare lights sources
  
  Lights lights {};
  lights.ambient_.emplace_back(FColor{255.0f, 255.0f, 255.0f}, 0.2f);
  lights.infinite_.emplace_back(FColor{255.0f, 255.0f, 0.0f}, 0.6f, Vector{-1.0f, -1.0f, 0.0f});

  // Other stuff

  Buffer  buf (kWidth, kHeight, color::Black);
  GlText  text {win};

  do {
    timer.Start();
    win.Clear();

    // Handle input

    auto kbtn = win.ReadKeyboardBtn(BtnType::KB_DOWN);
    helpers::HandleCamMovement(kbtn, cam);
    helpers::HandlePause(kbtn, win);

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
    auto culled = objects::Cull(cubes, cam);
    auto hidden = objects::RemoveHiddenSurfaces(cubes, cam);
    
    // Finally

    objects::World2Camera(cubes, cam);
    objects::RefreshFaceNormals(cubes);
    objects::RefreshVertexNormals(cubes);
    light::Objects(cubes, lights);
    objects::Camera2Persp(cubes, cam);
    objects::Homogenous2Normal(cubes);
    objects::Persp2Screen(cubes, cam);

    // Draw triangles (stored in object)

    buf.Clear();
    objects::SortZ(cubes);
    for (auto& cube : cubes)
      draw::SolidObject(cube, buf);
    buf.SendDataToFB();
    fps.Count();

    win.Render();
    timer.Wait();    

    if (fps.Ready())
    {
      std::cerr << "Frames per second: " << fps.ReadPrev() << '\n';
      std::cerr << "Objects culled: " << culled << '\n';
      std::cerr << "Hidden surface: " << hidden << "\n\n";
    }

  } while (!win.Closed());

  return 0;
}