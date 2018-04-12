// *******************************************************************
// File:    camera_operator.cc
// Descr:   used to manage camera movements, rotating and other stuff
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *******************************************************************

#include "camera_operator.h"

namespace anshub {

CameraOperator::CameraOperator(
  float fov,
  float dov, 
  int scr_w, 
  int scr_h,
  const Vector& vrp, 
  const Vector& dir,
  float z_near, 
  float z_far
)
  : GlCamera(fov, dov, scr_w, scr_h, vrp, dir, z_near, z_far)
  , move_left_ {KbdBtn::NONE}
  , move_right_ {KbdBtn::NONE}
  , move_forward_ {KbdBtn::NONE}
  , move_backward_ {KbdBtn::NONE}
  , move_up_ {KbdBtn::NONE}
  , move_down_ {KbdBtn::NONE}
  , jump_ {KbdBtn::NONE}
  , speed_up_ { KbdBtn::NONE}
  , zoom_in_ {KbdBtn::NONE}
  , zoom_out_ {KbdBtn::NONE}
  , switch_type_ {KbdBtn::NONE}
  , switch_roll_ {KbdBtn::NONE}
  , switch_wired_ {KbdBtn::NONE}
  , fly_ {KbdBtn::NONE}
  , roll_mode_{false}
  , wired_mode_{false}
  , fly_mode_{false}
  , speed_up_mode_{false}
  , speed_up_val_{cam_operator_consts::kSpeedUpDefault}
  , jump_height_{cam_operator_consts::kJumpDefault}
  , prev_mouse_pos_{-1,-1}
  , operator_height_{cam_operator_consts::kOperatorHeightDefault}
  , on_ground_{true}
  , mouse_sensitive_{1}
{ }

void CameraOperator::ProcessInput(const BaseWindow& win)
{
  // Start process speed

  speed_up_mode_ = IsButtonPressed(win, speed_up_);
  if (speed_up_mode_)
    accel_factor_ *= speed_up_val_;

  // Handle movements
  
  if (IsButtonPressed(win, fly_))
    fly_mode_ = !fly_mode_;  
  if (IsButtonPressed(win, move_forward_))
    this->MoveForward();
  if (IsButtonPressed(win, move_backward_))
    this->MoveBackward();
  if (IsButtonPressed(win, move_right_))
    this->MoveRight();
  if (IsButtonPressed(win, move_left_))
    this->MoveLeft();
  if (fly_mode_ && IsButtonPressed(win, move_up_))
    this->MoveUp();
  if (fly_mode_ && IsButtonPressed(win, move_down_))
    this->MoveDown();
  if (IsButtonPressed(win, zoom_in_))
    this->ChangeFov(this->fov_ - 1.0f);
  if (IsButtonPressed(win, zoom_out_))
    this->ChangeFov(this->fov_ + 1.0f);
  if (!fly_mode_ && on_ground_ && IsButtonPressed(win, jump_))
  {
    vel_.y = jump_height_;
    on_ground_ = false;
  }
  this->ProcessVelocity(fly_mode_, on_ground_);

#ifdef DEBUG
  std::cerr << "Accel (x,z): " << accel_.x << ' ' << accel_.z << '\n';
  std::cerr << "Vel   (x,z): " << vel_.x << ' ' << vel_.z << '\n';
  std::cerr << "Frict (x,z): " << frict_.x << ' ' << frict_.z << "\n\n";
#endif

  // End speed-up

  if (speed_up_mode_)
    accel_factor_ /= speed_up_val_;  

  // Handle swithching camera type

  if (IsButtonPressed(win, switch_type_))
  {
    if (this->type_ == GlCamera::Type::EULER)
      this->SwitchType(GlCamera::Type::UVN);
    else if (this->type_ == GlCamera::Type::UVN)
      this->SwitchType(GlCamera::Type::EULER);
  }

  // Handle switching wired mode

  if (IsButtonPressed(win, switch_wired_))
    wired_mode_ = !wired_mode_;

  // Handle roll mode

  if (IsButtonPressed(win, switch_roll_))
    roll_mode_ = !roll_mode_;

  // Handle camera rotating

  auto mpos = win.ReadMousePos();
  if (prev_mouse_pos_.x != -1 && prev_mouse_pos_.y != -1)
  {
    if (roll_mode_)
      this->dir_.z -= (prev_mouse_pos_.x - mpos.x) / mouse_sensitive_;
    else
      this->dir_.y -= (prev_mouse_pos_.x - mpos.x) / mouse_sensitive_;

    this->dir_.x -= (prev_mouse_pos_.y - mpos.y) / mouse_sensitive_;
  }
  prev_mouse_pos_ = mpos;
}

// Set position of camera on the given ypos

void CameraOperator::SetGroundPosition(float ypos)
{
  if (!fly_mode_)
  {
    this->vrp_.y += this->vel_.y;
    if (this->vrp_.y <= ypos + operator_height_)
    {
      this->vrp_.y = ypos + operator_height_;
      this->vel_.y = 0.0f;
      on_ground_ = true;
    }
    else
      on_ground_ = false;
  }
}

}  // namespace anshub