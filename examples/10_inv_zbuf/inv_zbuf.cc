// *************************************************************
// File:    inv_zbuf.cc
// Descr:   textured objects with 1/z buffer
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
#include "lib/math/segment.h"
#include "lib/math/trig.h"

#include "lib/render/gl_draw.h"
#include "lib/render/gl_text.h"
#include "lib/render/gl_lights.cc"
#include "lib/render/gl_coords.h"
#include "lib/render/gl_object.h"
#include "lib/render/gl_z_buffer.h"
#include "lib/render/cameras/gl_camera.h"

#include "lib/math/matrices/mx_rotate_eul.h"
#include "lib/math/matrices/mx_rotate_uvn.h"
#include "lib/math/matrices/mx_translate.h"

#include "../helpers.h"

using namespace anshub;
using namespace helpers;
using anshub::vector::operator<<;
using anshub::matrix::operator<<;

const char* HandleInput(int argc, const char** argv)
{
  if (argc != 2)
    return NULL;
  else
    return argv[1];
}

auto CreateGround(int rect_cnt, TrigTable& trig)
{
  auto master = object::Make("../00_data/objects/floor.ply", trig,
    {1.0f, 1.0f, 1.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f}
  );
  float item_size = 0.2f;
  float gap_size  = 5.0f;
  float step      = item_size + gap_size;
  float half_w    = (step) * (rect_cnt / 2);
  float half_h    = half_w;
  
  V_GlObject ground (rect_cnt * rect_cnt, master);
  auto it = ground.begin();
  for (auto z = -half_h; z < half_h; z += step)
  {
    for (auto x = -half_w; x < half_w; x += step)
    {
      Vector pos {x, 0.0f, z};
      it->world_pos_ = pos;
      object::Scale(*it, {item_size, item_size, item_size});      
      ++it;
    }
  }
  return ground;
}

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
  const char* fname = HandleInput(argc, argv);
  if (!fname) {
    std::cerr << "Incorrect file name\n";
    return 1;
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

  auto obj = object::Make(
    fname, trig, 
    {2.0f, 2.0f, 2.0f},     // initial scale
    {0.0f, 0.0f, 10.0f},    // world pos
    {0.0f, 0.0f, 0.0f}      // initial rotate
  );
  auto ground = CreateGround(20, trig);

  float    dov     {1.0f};
  float    fov     {75.0f};
  Vector   cam_pos {0.0f, 0.0f, -10.0f};
  Vector   cam_dir {0.0f, 0.0f, 0.0f};
  float    near_z  {dov};
  float    far_z   {500};
  auto camman = MakeCameraman(
    fov, dov, kWidth, kHeight, cam_pos, cam_dir, near_z, far_z, trig);

  Lights lights {};
  FColor white  {255.0f, 255.0f, 255.0f};
  FColor yellow {255.0f, 255.0f, 0.0f};
  FColor blue   {0.0f, 0.0f, 255.0f};

  lights.ambient_.emplace_back(white, 0.3f);
  lights.infinite_.emplace_back(white, 0.7f, Vector{-1.0f, -1.0f, 0.0f});
  lights.point_.emplace_back(blue, 0.6f, 
    Vector{0.0f, 0.0f, 10.0f}, Vector {0.0f, 0.0f, -1.0f});

  ScrBuffer buf (kWidth, kHeight, 0);
  ZBuffer   zbuf (kWidth, kHeight);
  GlText    text {win};
  Vector    obj_rot    {0.0f, 0.0f, 0.0f};

  auto tris_base = triangles::MakeBaseContainer(0);
  auto tris_ptrs = triangles::MakePtrsContainer(0);

  do {
    timer.Start();
    win.Clear();

    camman.ProcessInput(win);
    auto& cam = camman.GetCurrentCamera();

    Vector  obj_vel    {0.0f, 0.0f, 0.0f};
    Vector  obj_scale  {1.0f, 1.0f, 1.0f};

    auto kbtn = win.ReadKeyboardBtn(BtnType::KB_DOWN);
    HandlePause(kbtn, win);
    HandleObject(kbtn, obj_vel, obj_rot, obj_scale);

    obj.world_pos_ += obj_vel;

    obj.SetCoords(Coords::LOCAL);
    object::Rotate(obj, obj_rot, trig);
    obj.CopyCoords(Coords::LOCAL, Coords::TRANS);
    obj.SetCoords(Coords::TRANS);
    objects::CopyCoords(ground, Coords::LOCAL, Coords::TRANS);
    objects::SetCoords(ground, Coords::TRANS);
    object::Translate(obj, obj.world_pos_);
    for (auto& it : ground)
      object::Translate(it, it.world_pos_);
    
    object::ResetAttributes(obj);
    objects::ResetAttributes(ground);

    auto hidden = objects::RemoveHiddenSurfaces(ground, cam);
    hidden += object::RemoveHiddenSurfaces(obj, cam);

    object::ComputeFaceNormals(obj);
    object::ComputeVertexNormalsV2(obj);
    light::Object(obj, lights);

    MatrixCamera mx_cam {};
    if (cam.type_ == CamType::EULER)
    {
      MatrixTranslate   mx_cam_trans  {cam.vrp_ * (-1)};
      MatrixRotateEul   mx_cam_rot    {cam.dir_ * (-1), trig};
      mx_cam = matrix::Multiplie(mx_cam, mx_cam_trans);
      mx_cam = matrix::Multiplie(mx_cam, mx_cam_rot);
    }
    else
    {
      auto& uvn = camman.GetCamera(CamType::Uvn::type);      
      uvn.LookAt(obj.world_pos_);
      MatrixTranslate   mx_cam_trans  {cam.vrp_ * (-1)};
      MatrixRotateUvn   mx_cam_rot    {uvn.GetU(), uvn.GetV(), uvn.GetN()};
      mx_cam = matrix::Multiplie(mx_cam_trans, mx_cam_rot);
    }

    object::ApplyMatrix(mx_cam, obj);
    objects::ApplyMatrix(mx_cam, ground);

    tris_base.resize(0);
    tris_ptrs.resize(0);
    triangles::AddFromObjects(ground, tris_base);
    triangles::AddFromObject(obj, tris_base);
    auto culled = triangles::CullAndClip(tris_base, cam);    
    triangles::MakePointers(tris_base, tris_ptrs);
    triangles::SortZAvg(tris_ptrs);
    
    triangles::Camera2Persp(tris_base, cam);
    triangles::Persp2Screen(tris_base, cam);

    buf.Clear();
    zbuf.Clear();
    draw_triangles::Solid(tris_ptrs, zbuf, buf);
    buf.SendDataToFB();

    PrintInfo(
      text, fps, obj.world_pos_, obj_rot, cam.vrp_, cam.dir_, culled, hidden
    );
    fps.Count();

    win.Render();
    timer.Wait();

  } while (!win.Closed());

  return 0;
}
