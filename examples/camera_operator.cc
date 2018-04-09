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
  , roll_mode_{false}
  , wired_mode_{false}
  , fly_mode_{false}
  , speed_up_mode_{false}
  , speed_up_val_{cam_operator_consts::kSpeedUpDefault}
  , jump_height_{cam_operator_consts::kJumpDefault}
  , prev_mouse_pos_{-1,-1}
  , operator_height_{cam_operator_consts::kOperatorHeightDefault}
  , on_ground_{true}
  , gravity_ {cam_operator_consts::kGravityDefault}
  , mouse_sensitive_{1}
{ }

void CameraOperator::ProcessInput(const BaseWindow& win)
{

  // // Start process speed

  speed_up_mode_ = win.IsKeyboardBtnPressed(speed_up_);
  if (speed_up_mode_)
    accel_factor_ *= speed_up_val_;

  // Handle movements

  // float accel_factor = accel_factor_;//0.1f;
  // float frict_factor = frict_factor_;//0.8f;
  float kMaxAccel = 5.0f;

  if (win.IsKeyboardBtnPressed(move_forward_))
  {
    if (type_ == GlCamera::Type::EULER)
    {
      accel_.z += accel_factor_ * trig_.Cos(dir_.y);
      accel_.x += accel_factor_ * trig_.Sin(dir_.y);
    }
    else
      accel_.z += accel_factor_;
  }

  if (win.IsKeyboardBtnPressed(move_backward_))
  {
    if (type_ == GlCamera::Type::EULER)
    {
      accel_.z -= accel_factor_ * trig_.Cos(dir_.y);
      accel_.x -= accel_factor_ * trig_.Sin(dir_.y);
    }
    else
      accel_.z -= accel_factor_;
  }

  if (win.IsKeyboardBtnPressed(move_left_))
  {
    if (type_ == GlCamera::Type::EULER)
    {
      accel_.x -= accel_factor_ * trig_.Cos(dir_.y);
      accel_.z += accel_factor_ * trig_.Sin(dir_.y);
    }
    else
      accel_.x -= accel_factor_;
  }

  if (win.IsKeyboardBtnPressed(move_right_))
  {
    if (type_ == GlCamera::Type::EULER)
    {
      accel_.x += accel_factor_ * trig_.Cos(dir_.y);
      accel_.z -= accel_factor_ * trig_.Sin(dir_.y);
    }
    else
      accel_.x += accel_factor_;
  }

    if (win.IsKeyboardBtnPressed(move_up_) && fly_mode_)
  {
    accel_.y += accel_factor_;
  }

  if (win.IsKeyboardBtnPressed(move_down_) && fly_mode_)
  {
    accel_.y -= accel_factor_;
  }

  // std::cerr << "Accel (x,z): " << accel_.x << ' ' << accel_.z << '\n';
  // std::cerr << "Vel   (x,z): " << vel_.x << ' ' << vel_.z << '\n';
  // std::cerr << "Frict (x,z): " << frict_.x << ' ' << frict_.z << "\n\n";

  // Process velocity
  
  // if (!vel_.IsZero())
  // {
    if (fly_mode_)
    {
      accel_ *= frict_factor_;
      vel_ += accel_;
      vel_ *= frict_factor_;
      vrp_ += vel_;
    }
    else {
      accel_.x *= frict_factor_;
      accel_.z *= frict_factor_;
      vel_ += accel_;
      vel_.x *= frict_factor_;
      vel_.z *= frict_factor_;
      vrp_ += vel_;

      if (!on_ground_)
        vel_.y += gravity_;     // process gravity
    }
  // }
  // else {
  //   accel_.Zero();
  //   vel_.Zero();
  // }
  // accel_ += frict_;
  // if (accel_.x < 0.0f || accel_.z < 0.0f)
  // {
  //   accel_.Zero();
  //   frict_.Zero();
  // }
  // if (!vel_.IsZero())

  // std::cerr << 
  
  // Process friction

  // accel_ *= friction_factor_;
  // accel_ *= -1.0f;
  // accel_ *= frict_factor;
  



    // vel_.z *= speed_up_val_;

  // // Handle movements
  
  // if (win.IsKeyboardBtnPressed(move_forward_)) this->MoveForward();
  // if (win.IsKeyboardBtnPressed(move_backward_)) this->MoveBackward();
  // if (win.IsKeyboardBtnPressed(move_right_)) this->MoveRight();
  // if (win.IsKeyboardBtnPressed(move_left_)) this->MoveLeft();
  // if (win.IsKeyboardBtnPressed(move_up_)) this->MoveUp();
  // if (win.IsKeyboardBtnPressed(move_down_)) this->MoveDown();
  if (win.IsKeyboardBtnPressed(zoom_in_)) this->ChangeFov(this->fov_ - 1.0f);
  if (win.IsKeyboardBtnPressed(zoom_out_)) this->ChangeFov(this->fov_ + 1.0f);
  if (win.IsKeyboardBtnPressed(jump_) && !fly_mode_ && on_ground_)
  {
    vel_.y = jump_height_;    // todo : magic const
    on_ground_ = false;
  }

  // End speed-up

  if (speed_up_mode_)
    accel_factor_ /= speed_up_val_;  
    // vel_.z /= speed_up_val_;

  // if (!on_ground_)
  // {
  //   std::cerr << "PRocGrav\n";
  //   ProcessGravity();
  // }

  // Handle swithching camera type

  if (win.IsKeyboardBtnPressed(switch_type_))
  {
    if (this->type_ == GlCamera::Type::EULER)
      this->SwitchType(GlCamera::Type::UVN);
    else if (this->type_ == GlCamera::Type::UVN)
      this->SwitchType(GlCamera::Type::EULER);
  }

  // Handle switching wired mode

  if (win.IsKeyboardBtnPressed(switch_wired_))
    wired_mode_ = !wired_mode_;

  // Handle roll mode

  if (win.IsKeyboardBtnPressed(switch_roll_))
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
    if (this->vrp_.y < ypos + operator_height_)
    {
      this->vrp_.y = ypos + operator_height_;
      this->vel_.y = 0.0f;
      on_ground_ = true;
    }
    else
      on_ground_ = false;
  }
}

// Apply gravity vector to velocity vector (should be used every frame
// after camera movements)

void CameraOperator::ProcessGravity()
{
  vel_.y += gravity_;
  std::cerr << vel_.y << '\n';
}

}  // namespace anshub