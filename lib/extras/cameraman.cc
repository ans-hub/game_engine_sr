// *******************************************************************
// File:    camera_operator.cc
// Descr:   used to manage camera movements, rotating and other stuff
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *******************************************************************

#include "cameraman.h"

namespace anshub {

void CameraMan::ProcessInput(const BaseWindow& win)
{
  // Camera-type specific preprocessing (virtual)

  cam_->Preprocess();

  // Preprocess values

  bool mode_speedup = IsButtonPressed(win, Btn::SPEED_UP);
  auto mode_fly = GetState(CamState::FLY_MODE);
  auto mode_wired = GetState(CamState::WIRED_MODE);
  auto mode_roll = GetState(CamState::ROLL_MODE);
  auto mode_bifilt = GetState(CamState::BIFILTERING_MODE);
  auto mode_mipmap = GetState(CamState::MIPMAP_MODE);
  auto state_onground = GetState(CamState::ON_GROUND);

  // Start process speed

  if (mode_speedup)
  {
    float factor = GetValue(CamValue::SPEED_UP);
    dyn_.SpeedUp(factor);
  }

  // Handle movements
  
  if (IsButtonPressed(win, Btn::MOVE_FORWARD))
  {
    if (cam_->type_ != CamType::FOLLOW)
      dyn_.MoveForward(cam_->dir_);
    else
      dyn_.MoveNorth();
  }
  if (IsButtonPressed(win, Btn::MOVE_BACKWARD))
  {
    if (cam_->type_ != CamType::FOLLOW)
      dyn_.MoveBackward(cam_->dir_);
    else
      dyn_.MoveSouth();
  }
  if (IsButtonPressed(win, Btn::STRAFE_LEFT))
  {
    if (cam_->type_ != CamType::FOLLOW)
      dyn_.StrafeLeft(cam_->dir_);
    else
      dyn_.MoveWest();
  }
  if (IsButtonPressed(win, Btn::STRAFE_RIGHT))
  {
    if (cam_->type_ != CamType::FOLLOW)
      dyn_.StrafeRight(cam_->dir_);
    else
      dyn_.MoveEast();
  }
  if (IsButtonPressed(win, Btn::TURN_LEFT) && cam_->type_ != CamType::UVN)
    dyn_.RotateYaw(cam_->yaw_.vel_);
  if (IsButtonPressed(win, Btn::TURN_RIGHT) && cam_->type_ != CamType::UVN)
    dyn_.RotateYaw(-cam_->yaw_.vel_);
  if (IsButtonPressed(win, Btn::LOOK_UP) && cam_->type_ != CamType::UVN)
    dyn_.RotatePitch(-cam_->pitch_.vel_);
  if (IsButtonPressed(win, Btn::LOOK_DOWN) && cam_->type_ != CamType::UVN)
    dyn_.RotatePitch(cam_->pitch_.vel_);        
  if (GetState(CamState::FLY_MODE) && IsButtonPressed(win, Btn::MOVE_UP))
    dyn_.MoveUp();
  if (GetState(CamState::FLY_MODE) && IsButtonPressed(win, Btn::MOVE_DOWN))
    dyn_.MoveDown();
  if (IsButtonPressed(win, Btn::ZOOM_IN))
    cam_->ChangeFov(cam_->fov_ - 1.0f);
  if (IsButtonPressed(win, Btn::ZOOM_OUT))
    cam_->ChangeFov(cam_->fov_ + 1.0f);
  if (!mode_fly && state_onground && IsButtonPressed(win, Btn::JUMP))
  {
    auto height = GetValue(CamValue::JUMP_HEIGHT);
    dyn_.Jump(height);
    SetState(CamState::ON_GROUND, false);
  }

  // Process total velocity after user input

  dyn_.ProcessVelocity(mode_fly, GetState(CamState::ON_GROUND));
  cam_->vrp_ += dyn_.GetVelocity();

  // End speed-up

  if (mode_speedup)
  {
    float factor = GetValue(CamValue::SPEED_UP);
    dyn_.SpeedDown(factor);
  }

  // Handle swithching camera type

  if (IsButtonPressed(win, Btn::SWITCH_TYPE))
  {
    if (cam_->type_ == CamType::EULER)
    {
      cam_uvn_.vrp_ = cam_->vrp_;
      cam_uvn_.dir_ = cam_->dir_;
      cam_uvn_.Reinitialize();
      cam_ = &cam_uvn_;
    }
    else if (cam_->type_ == CamType::UVN)
    {
      cam_eul_.vrp_ = cam_->vrp_;
      cam_eul_.dir_ = cam_->dir_;
      cam_= &cam_eul_;
    }
  }

  // Handle other stuff

  if (IsButtonPressed(win, Btn::FLY_MODE))
  {
    mode_fly = !mode_fly;
    SetState(CamState::FLY_MODE, mode_fly);
    dyn_.ResetAcceleration();
    dyn_.ResetVelocity();
    if (mode_fly)
      SetState(CamState::ON_GROUND, false);
  }

  if (IsButtonPressed(win, Btn::WIRED))
    SetState(CamState::WIRED_MODE, !mode_wired);

  if (IsButtonPressed(win, Btn::ROLL_MODE))
    SetState(CamState::ROLL_MODE, !mode_roll);
  
  if (IsButtonPressed(win, Btn::BIFILTERING_MODE))
    SetState(CamState::BIFILTERING_MODE, !mode_bifilt);
  
  if (IsButtonPressed(win, Btn::MIPMAP_MODE))
    SetState(CamState::MIPMAP_MODE, !mode_mipmap);

  // Handle camera rotating

  auto mpos = win.ReadMousePos();
  auto mouse_sensitive = GetValue(CamValue::MOUSE_SENSITIVE);
  if (prev_mouse_pos_.x != -1 && prev_mouse_pos_.y != -1)
  {
    auto yaw = (prev_mouse_pos_.x - mpos.x) / mouse_sensitive;
    if (mode_roll)
      dyn_.RotateRoll(yaw);
    else
      dyn_.RotateYaw(yaw);
    
    auto pitch = (prev_mouse_pos_.y - mpos.y) / mouse_sensitive;
    dyn_.RotatePitch(pitch);
  }
  prev_mouse_pos_ = mpos;

  // Process total smooth direction change

  dyn_.ProcessDirVelocity();
  cam_->dir_ += dyn_.GetDirVelocity();
}

// Set position of camera on the given ypos

void CameraMan::SetGroundPosition(float ypos)
{
  auto mode_fly = GetState(CamState::FLY_MODE);
  auto operator_height = GetValue(CamValue::OPERATOR_HEIGHT);

  if (!mode_fly)
  {
    cam_->vrp_.y += dyn_.GetVelocity().y;
    if (cam_->vrp_.y <= ypos + operator_height)
    {
      cam_->vrp_.y = ypos + operator_height;
      SetState(CamState::ON_GROUND, true);
    }
    else
      SetState(CamState::ON_GROUND, false);
  }
}

}  // namespace anshub