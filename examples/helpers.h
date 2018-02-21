// *************************************************************
// File:    helpers.h
// Descr:   input-output helpers
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#include <iostream>

#include "lib/window/base_window.h"
#include "lib/window/gl_window.h"
#include "lib/draw/gl_camera.h"

using namespace anshub;

namespace helpers {

const char* HandleInput(int argc, const char** argv)
{
  if (argc != 2)
    return NULL;
  else
    return argv[1];
}

void HandleCamMovement(Btn kbtn, GlCamera& cam)
{
  if (kbtn == Btn::W)
    cam.vrp_.z += 0.5f;
  else if (kbtn == Btn::S)
    cam.vrp_.z -= 0.5f;
  if (kbtn == Btn::R)
    cam.vrp_.y += 0.5f;
  else if (kbtn == Btn::F)
    cam.vrp_.y -= 0.5f;
  if (kbtn == Btn::A)
    cam.vrp_.x -= 0.5f;
  else if (kbtn == Btn::D)
    cam.vrp_.x += 0.5f;
}

void HandlePause(Btn key, GlWindow& win)
{
  if (key == Btn::P)
  {
    Timer timer {};
    timer.SetMillisecondsToWait(100);
    while (true)
    {
      if (win.ReadKeyboardBtn(BtnType::KB_DOWN) == Btn::P)
        break;
      else
        timer.Wait();
    }
  }
}

} // namespace helpers