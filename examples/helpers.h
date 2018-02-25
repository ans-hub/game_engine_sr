// *************************************************************
// File:    helpers.h
// Descr:   input-output helpers
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#include <iostream>

#include "lib/system/timer.h"
#include "lib/draw/gl_camera.h"
#include "lib/window/base_window.h"
#include "lib/window/gl_window.h"

namespace anshub {

namespace helpers {

  void HandleCamType(Btn, GlCamera&);
  void HandleCamMovement(Btn, GlCamera&);
  void HandleCamRotate(bool mode, const Pos&, Pos&, Vector&);
  void HandlePause(Btn, GlWindow&);
  void HandleObject(Btn, Vector&, Vector&, Vector&);
  void HandleRotateObject(Btn, Vector&);

} // namespace helpers

} // namespace anshub