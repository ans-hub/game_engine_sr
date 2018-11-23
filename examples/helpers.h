// *************************************************************
// File:    helpers.h
// Descr:   input-output helpers
// Author:  Novoselov Anton @ 2017
// *************************************************************

#ifndef GC_HELPERS_CAMERA_H
#define GC_HELPERS_CAMERA_H

#include <iostream>
#include <sstream>

#include "lib/math/vector.h"

#include "lib/physics/physics.h"

#include "lib/extras/cameraman.h"

#include "lib/render/gl_text.h"

#include "lib/window/base_window.h"
#include "lib/window/gl_window.h"

#include "lib/system/timer.h"
#include "lib/system/fps_counter.h"

namespace anshub {

namespace helpers {

  // Execute helpers

  template<class ... Args> auto MakeCameraman(Args&&...);
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

// Returns cameraman with predefined keys

template<class ... Args>
inline auto helpers::MakeCameraman(Args&&... args)
{
  CameraMan man (std::forward<Args>(args)...);
  man.SetButton(CamAction::STRAFE_LEFT, KbdBtn::A);
  man.SetButton(CamAction::STRAFE_RIGHT, KbdBtn::D);
  man.SetButton(CamAction::MOVE_FORWARD, KbdBtn::W);
  man.SetButton(CamAction::MOVE_BACKWARD, KbdBtn::S);
  man.SetButton(CamAction::MOVE_UP, KbdBtn::R);
  man.SetButton(CamAction::MOVE_DOWN, KbdBtn::F);
  man.SetButton(CamAction::TURN_LEFT, KbdBtn::NUM1);
  man.SetButton(CamAction::TURN_RIGHT, KbdBtn::NUM2);
  man.SetButton(CamAction::LOOK_UP, KbdBtn::NUM3);
  man.SetButton(CamAction::LOOK_DOWN, KbdBtn::NUM4);
  man.SetButton(CamAction::JUMP, KbdBtn::SPACE);
  man.SetButton(CamAction::ZOOM_IN, KbdBtn::NUM9);
  man.SetButton(CamAction::ZOOM_OUT, KbdBtn::NUM0);
  man.SetButton(CamAction::ROLL_MODE, KbdBtn::L, 20);
  man.SetButton(CamAction::WIRED, KbdBtn::T, 20);
  man.SetButton(CamAction::SWITCH_TYPE, KbdBtn::BACKS, 20);
  man.SetButton(CamAction::SPEED_UP, KbdBtn::LSHIFT);

  man.SetState(CamState::FLY_MODE, true);

  man.SetValue(CamValue::MOUSE_SENSITIVE, 1.0f);  
  man.SetValue(CamValue::SPEED_UP, 5.0f);  

  Physics dyn {0.01f, 0.9f, -0.1f, 100.0f};
  man.SetPhysics(std::move(dyn));

  return man;
}

} // namespace anshub

#endif // GC_HELPERS_CAMERA_H