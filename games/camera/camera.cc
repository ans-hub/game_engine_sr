// *************************************************************
// File:    camera.cc
// Descr:   camera example
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#include <fstream>
#include <tuple>
#include <cmath>

#include "lib/window/gl_window.h"
#include "lib/window/helpers.h"
#include "lib/system/rand_toolkit.h"
#include "lib/system/timer.h"
#include "lib/math/matrix_rotate.h"
#include "lib/math/matrix_persp.h"
#include "lib/math/matrix_trans.h"
#include "lib/math/matrix_view.h"
#include "lib/math/matrix_scale.h"
#include "lib/draw/gl_draw.h"
#include "lib/draw/gl_object.h"
#include "lib/math/vector.h"
#include "lib/math/segment.h"
#include "lib/math/trig.h"

using namespace anshub;

int main()
{
  // Math processor
  
  rand_toolkit::start_rand();
  TrigTable trig {};

  // Window

  constexpr int kWidth = 800;
  constexpr int kHeight = 600;
  constexpr int kFpsWait = 35;
  auto pos  = io_helpers::GetXYToMiddle(kWidth, kHeight); 
  GlWindow win (pos.x, pos.y, kWidth, kHeight, "Camera"); 
  Timer    timer (kFpsWait);

  // Objects

  Vector    scale  {1.0f, 1.0f, 1.0f};
  Vector    objpos {0.0f, 0.0f, 100.0f};
  GlObject  cube = object::Make("cube.ply", scale, objpos);

  // Camera (dov - dist of view, fov - field of view, wov - width of view)

  float dov = 2;
  float fov = 60;
  float ar = (float)kWidth / (float)kHeight;
  float wov = 2 * trig::CalcOppositeCatet(dov, fov/2, trig);

  Buffer buf (kWidth, kHeight, 0);
  Vector cube_rot (0.0f, 0.0f, 0.0f);

  do {
    timer.Start();
    win.Clear();

    // Move cube
    
    Vector cube_vel (0.0f, 0.0f, 0.0f);

    // HandleKeyboard();
    auto kbtn = win.ReadKeyboardBtn(BtnType::KB_DOWN);
    switch(kbtn)
    {
      case Btn::W : cube_vel.z = 1.0f; break;
      case Btn::S : cube_vel.z = -1.0f; break;
      case Btn::UP : cube_vel.y = 1.0f; break;
      case Btn::DOWN : cube_vel.y = -1.0f; break;
      case Btn::LEFT : cube_vel.x = -1.0f; break;
      case Btn::RIGHT : cube_vel.x = 1.0f; break;
      case Btn::Z : cube_rot.x -= 1.0f; break;
      case Btn::X : cube_rot.x += 1.0f; break;
      case Btn::C : cube_rot.y -= 1.0f; break;
      case Btn::V : cube_rot.y += 1.0f; break;
      case Btn::B : cube_rot.z -= 1.0f; break;
      case Btn::N : cube_rot.z += 1.0f; break;
      case Btn::SPACE : 
        while (true)
        {
          if (win.ReadKeyboardBtn(BtnType::KB_DOWN) == Btn::SPACE)
            break;
          else
            timer.Wait();
        }
        break;
      default : break;
    }

    cube.world_pos_ += cube_vel;

    MatrixRotate mx_rot {cube_rot, trig};
    MatrixTranslate mx_trans {cube.world_pos_};
    MatrixPerspective mx_per {dov, ar};
    MatrixScale mx_scale(1.0f);

    cube.SetCoords(Coords::LOCAL);
    object::ApplyMatrix(mx_rot, cube);
    cube.CopyCoords(Coords::LOCAL, Coords::TRANS);

    auto mx_total = matrix::Multiplie(mx_trans, mx_per);
    mx_total = matrix::Multiplie(mx_total, mx_scale);
    cube.SetCoords(Coords::TRANS);
    object::ApplyMatrix(mx_total, cube);
 
    for (auto& vx : cube.GetCoords())
      vector::ConvertFromHomogeneous(vx);

    MatrixViewport mx_view {wov, kWidth, kHeight};
    object::ApplyMatrix(mx_view, cube);

    // Draw triangles (stored in object)

    buf.Clear();
    draw::Object(cube, kWidth, kHeight, buf);
    buf.SendDataToFB();

    win.Render();
    timer.Wait();

  } while (!win.Closed());

  return 0;
}