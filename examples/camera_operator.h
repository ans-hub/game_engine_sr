// *******************************************************************
// File:    camera_operator.h
// Descr:   used to manage camera movements, rotating and other stuff
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *******************************************************************

#ifndef GC_CAMERA_OPERATOR_H
#define GC_CAMERA_OPERATOR_H

#include "camera_button.h"
#include "lib/window/enums.h"
#include "lib/window/base_window.h"
#include "lib/draw/gl_camera.h"
#include "lib/math/vector.h"

namespace anshub {

// Camera operator - performs camera movements and other features

struct CameraOperator : public GlCamera
{
  CameraOperator(
    float fov, float dov, int scr_w, int scr_h,
    const Vector& vrp, const Vector& dir,
    float z_near, float z_far
  );
  CameraOperator(const CameraOperator&) =default;
  CameraOperator& operator=(const CameraOperator&) =default;
  CameraOperator(CameraOperator&&) =default;
  CameraOperator& operator=(CameraOperator&&) =default;
  ~CameraOperator() noexcept override { }
  
  void ProcessInput(const BaseWindow&);
  bool IsWired() const { return wired_mode_; }
  bool IsOnGround() const { return on_ground_; }

  // Button setters

  void SetPrevMousePos(const Pos& pos) { prev_mouse_pos_ = pos; }
  void SetLeftButton(KbdBtn b, int delay = 0)
    { move_left_ = CameraBtn(b, delay); }
  void SetRightButton(KbdBtn b, int delay = 0)
    { move_right_ = CameraBtn(b, delay); }
  void SetForwardButton(KbdBtn b, int delay = 0)
    { move_forward_ = CameraBtn(b, delay); }
  void SetBackwardButton(KbdBtn b, int delay = 0)
    { move_backward_ = CameraBtn(b, delay); }
  void SetUpButton(KbdBtn b, int delay = 0)
    { move_up_ = CameraBtn(b, delay); }
  void SetDownButton(KbdBtn b, int delay = 0)
    { move_down_ = CameraBtn(b, delay); }
  void SetJumpButton(KbdBtn b, int delay = 0)
    { jump_ = CameraBtn(b, delay); }
  void SetSpeedUpButton(KbdBtn b, int delay = 0)
    { speed_up_ = CameraBtn(b, delay); }
  void SetZoomInButton(KbdBtn b, int delay = 0)
    { zoom_in_ = CameraBtn(b, delay); }
  void SetZoomOutButton(KbdBtn b, int delay = 0)
    { zoom_out_ = CameraBtn(b, delay); }
  void SetSwitchTypeButton(KbdBtn b, int delay = 0)
    { switch_type_ = CameraBtn(b, delay); }
  void SetSwitchRollButton(KbdBtn b, int delay = 0)
    { switch_roll_ = CameraBtn(b, delay); }
  void SetFlyModeButton(KbdBtn b, int delay = 0)
    { fly_ = CameraBtn(b, delay); }
  void SetWiredModeButton(KbdBtn b, int delay = 0)
    { switch_wired_ = CameraBtn(b, delay); }

  // State setters

  void SetGroundPosition(float ypos);
  void SetOnGround(bool s) { on_ground_ = s; }
  void SetOperatorHeight(float v) { operator_height_ = v; }
  void SetSpeedUpValue(float s) { speed_up_val_ = s; }
  void SetJumpHeight(float h) { jump_height_ = h; }
  void SetFlyMode(bool m) { fly_mode_ = m; }
  void SetMouseSensitive(int s) { mouse_sensitive_ = s; }

protected:

  bool IsButtonPressed(const BaseWindow&, CameraBtn&);

  CameraBtn move_left_;
  CameraBtn move_right_;
  CameraBtn move_forward_;
  CameraBtn move_backward_;
  CameraBtn move_up_;
  CameraBtn move_down_;
  CameraBtn jump_;
  CameraBtn speed_up_;
  CameraBtn zoom_in_;
  CameraBtn zoom_out_;
  CameraBtn switch_type_;     // euler or uvn
  CameraBtn switch_roll_;     // switch on/off roll mode
  CameraBtn switch_wired_;
  CameraBtn fly_;
  bool      roll_mode_;       // if true, then cam rotate z axis, else x
  bool      wired_mode_;      // if true, then all object are draws as wired
  bool      fly_mode_;        // if fly, then SetGroundPosition is ignored 
  bool      speed_up_mode_;
  float     speed_up_val_;
  float     jump_height_;
  Pos       prev_mouse_pos_;  // used to determine where was mouse in prev frame
  float     operator_height_; // distance of camera from the ground
  float     prev_ypos_;
  bool      on_ground_;
  int       mouse_sensitive_;

}; // class CameraOperator 

inline bool CameraOperator::IsButtonPressed(const BaseWindow& w, CameraBtn& b)
{
  if (!b.Delay() && w.IsKeyboardBtnPressed(b.btn_))
  {
    b.ResetDelay();
    return true;
  }
  return false;
}

namespace cam_operator_consts {

  const float kSpeedUpDefault {3.0f};
  const float kJumpDefault {3.0f};
  const float kOperatorHeightDefault {4.0f};

} // namespace cam_operator_consts

} // namespace anshub

#endif // GC_CAMERA_OPERATOR_H