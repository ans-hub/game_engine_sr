// *************************************************************
// File:    helpers.h
// Descr:   input-output helpers
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef GC_HELPERS_CAMERA_H
#define GC_HELPERS_CAMERA_H

#include <iostream>
#include <sstream>

#include "lib/window/base_window.h"
#include "lib/window/gl_window.h"
#include "lib/system/timer.h"
#include "lib/system/fps_counter.h"
#include "lib/draw/gl_camera.h"
#include "lib/draw/gl_text.h"
#include "lib/math/vector.h"

using anshub::vector::operator<<;

namespace anshub {

namespace helpers {

  // Execute helpers

  void HandleCamType(Btn, GlCamera&);
  void HandleCamMovement(Btn, float, GlCamera&);
  void HandleCamYPosition(float, GlCamera&);  
  void HandleCamRotate(bool mode, const Pos&, Pos&, Vector&);
  void HandleFullscreen(Btn, int mode, GlWindow&);
  void HandlePause(Btn, GlWindow&);
  void HandleObject(Btn, Vector&, Vector&, Vector&);
  void HandleRotateObject(Btn, Vector&);

  // Debug info helpers

  void PrintInfoOnScreen(
    GlText&, FpsCounter&,
    cVector& obj_pos, cVector& obj_rot,
    cVector& cam_pos, cVector& cam_rot,
    int nfo_culled, int nfo_hidden
  );
  void PrintInfoOnCmd(FpsCounter&, int culled, int hidden, cVector& world_pos);
  void PrintFpsOnCmd(FpsCounter&);

} // namespace helpers

} // namespace anshub

#endif // GC_HELPERS_CAMERA_H