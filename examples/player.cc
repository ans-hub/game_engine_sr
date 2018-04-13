// *************************************************************
// File:    player.cc
// Descr:   represents player in game
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#include "player.h"

namespace anshub {

Player::Player(
      GlObject&& model, CameraOperator& cam,
      float player_height, float cam_height, cVector& dir)
: CameraOperator(cam)
, obj_{std::move(model)}
, vrp_orig_{}
, cam_height_{cam_height}
, dir_accel_{}
, keys_{}
{
  this->operator_height_ = player_height;
  this->dir_ = dir;
}

void Player::SetButton(Action action, KbdBtn btn)
{
  keys_[action] = btn;
}

void Player::ProcessInput(BaseWindow& win, GlCamera& cam)
{
  // Start process speed

  bool speed_up_mode_ = win.IsKeyboardBtnPressed(keys_[SPEED_UP]);
  if (speed_up_mode_)
    accel_factor_ *= speed_up_val_;

  // Input

  if (win.IsKeyboardBtnPressed(keys_[MOVE_FORWARD]))
    this->MoveForward();

  if (win.IsKeyboardBtnPressed(keys_[MOVE_BACKWARD]))
    this->MoveBackward();

  if (win.IsKeyboardBtnPressed(keys_[STRAFE_LEFT]))
    this->MoveLeft();

  if (win.IsKeyboardBtnPressed(keys_[STRAFE_RIGHT]))
    this->MoveRight();

  // if (win.IsKeyboardBtnPressed(keys_[MOVE_UP])) {
  //   obj_.world_pos_.y += 1.0f;
  //   vrp_orig_.y += 1.0f;
  // }

  // if (win.IsKeyboardBtnPressed(keys_[MOVE_DOWN])) {
  //   obj_.world_pos_.y -= 1.0f;
  //   vrp_orig_.y -= 1.0f;
  // }

  // Direction of camera are independ of jeep direction, but syncronized

  if (win.IsKeyboardBtnPressed(keys_[TURN_LEFT])) {
    dir_.y -= 1.0f;
    cam.dir_.y -= 1.0f; 
  }

  if (win.IsKeyboardBtnPressed(keys_[TURN_RIGHT])) {
    dir_.y += 1.0f;
    cam.dir_.y += 1.0f;
  }

  if (win.IsKeyboardBtnPressed(keys_[LOOK_UP])) {
    dir_.x -= 1.0f;
    cam.dir_.x -= 1.0f;
  }
  
  if (win.IsKeyboardBtnPressed(keys_[LOOK_DOWN])) {
    dir_.x += 1.0f;
    cam.dir_.x += 1.0f;
  }

  if (win.IsKeyboardBtnPressed(keys_[TILT_LEFT])) {
    dir_.z -= 1.0f;
    cam.dir_.z -= 1.0f;
  }

  if (win.IsKeyboardBtnPressed(keys_[TILT_RIGHT])) {
    dir_.z += 1.0f;
    cam.dir_.z += 1.0f;
  }

  // End speed-up

  if (speed_up_mode_)
    accel_factor_ /= speed_up_val_;  

  // Process velocity

  this->ProcessVelocity(false, on_ground_);
  obj_.world_pos_ += vel_;
  vrp_orig_ += vel_;
}

namespace player {

// Initializes camera position and direction, and store vrp_origin

void InitCamera(Player& p, GlCamera& cam, float z_off, float pitch)
{
  // Init camera vrp

  auto dx = p.obj_.v_orient_z_ * (z_off);
  cam.vrp_ = p.obj_.world_pos_ + dx;
  p.vrp_orig_ = cam.vrp_;

  // Init camera dir

  cam.dir_ = p.dir_;
  cam.dir_.x += pitch;
}

// Get input from camera, and recompute camera height

void ProcessCameraInput(Player& p, CameraOperator& cam, BaseWindow& win)
{
  // Before new input we should restore old vrp (as now vrp is rotated)

  cam.vrp_ = p.vrp_orig_;
  cam.ProcessInput(win);
  
  // Recompute new camera height and then store new vrp as old

  p.cam_height_ += cam.vrp_.y - p.vrp_orig_.y;
  p.vrp_orig_ = cam.vrp_;
}

// Get input from player

void ProcessPlayerInput(Player& p, GlCamera& cam, BaseWindow& win)
{
  p.ProcessInput(win, cam);
}

// Find new ground position and renew vrp position

void ProcessGroundPosition(Player& p, const Terrain& terrain)
{
  float ground = terrain.FindGroundPosition(p.obj_.world_pos_);    
  
  p.vrp_ = p.obj_.world_pos_;
  p.SetGroundPosition(ground);
  p.obj_.world_pos_.y = p.vrp_.y;   // copy as in SGP we have res in vrp
  p.vrp_orig_.y = p.obj_.world_pos_.y + p.cam_height_;
}

// Find current ground normal and apply it to player direction

void ProcessGroundDirection(Player& p, GlCamera& cam, const Terrain& terrain)
{
  // const float kPitchRange {20.0f};
  // const float kRollRange {25.0f};
  // const float kRollRange {20.0f};
  // p.gimbal_.steps_ = 
  // if (p.IsOnGround())
  // {
    auto normal = terrain.FindGroundNormal(p.obj_.world_pos_);
    normal.Normalize();
    
    // Process pitch acceleration

    auto& or_z = p.obj_.v_orient_z_;
    or_z.Normalize();

    auto angle_x = (90.0f - vector::AngleBetween(normal, or_z, true));
    angle_x /= 4;
    if (math::Fzero(angle_x))
      p.dir_accel_.x = 0.0f;
    else
    {
      // if (angle_x > 1.0f)
      //   p.dir_accel_.x = 1.0f;
      // else if (angle_x < -1.0f)
      //   p.dir_accel_.x = -1.0f;
      // else
      //   p.dir_accel_.x = angle_x;
      p.dir_accel_.x = angle_x / 4.0f;
    }
      // p.dir_accel_.x = angle_x / 2.0f; // steps

    p.dir_.x += p.dir_accel_.x;
    p.dir_.x = std::max(-20.0f, p.dir_.x);
    p.dir_.x = std::min(20.0f, p.dir_.x);
    cam.dir_.x += p.dir_accel_.x / 1.2f;
    cam.dir_.x = std::max(-20.0f, cam.dir_.x);
    cam.dir_.x = std::min(20.0f, cam.dir_.x);
    // p.dir_.x = angle_x / 4;

    // Process roll acceleration

    auto& or_x = p.obj_.v_orient_x_;
    or_x.Normalize();

    auto angle_z = 90.0f - vector::AngleBetween(normal, or_x, true);
    angle_z /= 2;
    if (math::Fzero(angle_z))
      p.dir_accel_.z = 0.0f;
    else
    {
      // if (angle_z > 1.0f)
      //   p.dir_accel_.z = 1.0f;
      // else if (angle_z < -1.0f)
      //   p.dir_accel_.z = -1.0f;
      // else
      //   p.dir_accel_.z = angle_z;
      p.dir_accel_.z = angle_z / 4.0f;      
    }
      // p.dir_accel_.z = angle_z / 2.0f; // steps
    p.dir_.z += p.dir_accel_.z;
    p.dir_.z = std::max(-10.0f, p.dir_.z);
    p.dir_.z = std::min(10.0f, p.dir_.z);
    cam.dir_.z += p.dir_accel_.z / 1.2f;
    cam.dir_.z = std::max(-10.0f, p.dir_.z);
    cam.dir_.z = std::min(10.0f, p.dir_.z);
    
}

// Refresh orientation of player by reset orientation vectors to default
// and rotating them to the given direction by XYZ sequence

void ProcessPlayerOrientation(Player& p)
{
  auto& or_x = p.obj_.v_orient_x_;
  auto& or_y = p.obj_.v_orient_y_;
  auto& or_z = p.obj_.v_orient_z_;

  // Restore original orientation (always as origin)
  
  or_x = {1.0f, 0.0f, 0.0f};
  or_y = {0.0f, 1.0f, 0.0f};
  or_z = {0.0f, 0.0f, 1.0f};

  // Rotate orientation vectors by given direction in XYZ seq

  coords::RotatePitch(or_x, p.dir_.x, p.trig_);
  coords::RotateYaw(or_x, p.dir_.y, p.trig_);
  coords::RotateRoll(or_x, p.dir_.z, p.trig_);
  coords::RotatePitch(or_y, p.dir_.x, p.trig_);
  coords::RotateYaw(or_y, p.dir_.y, p.trig_);
  coords::RotateRoll(or_y, p.dir_.z, p.trig_);
  coords::RotatePitch(or_z, p.dir_.x, p.trig_);
  coords::RotateYaw(or_z, p.dir_.y, p.trig_);
  coords::RotateRoll(or_z, p.dir_.z, p.trig_);
}

// Rotate player coordinates by the XYZ (!) sequence. Note that we rotate
// not local coordinates to get right rotating sequence based on global
// player direction (dir_)

void ProcessPlayerRotating(Player& p)
{
  p.obj_.SetCoords(Coords::TRANS);
  p.obj_.CopyCoords(Coords::LOCAL, Coords::TRANS);

  for (auto& v : p.obj_.vxs_trans_)
  {
    coords::RotatePitch(v.pos_, p.dir_.x, p.trig_);
    coords::RotateYaw(v.pos_, p.dir_.y, p.trig_);
    coords::RotateRoll(v.pos_, p.dir_.z, p.trig_);
  }
}

// Move vrp so that it looks at the object befor object rotating

void ProcessCameraRotating(Player& p, GlCamera& cam)
{
  // We store original vrp since rotated vrp is necessary only for rendering.
  // If we don`t, then in next frame any user camera input would be applied to
  // incorrect vrp

  // Get obj2cam vector, then rotate its copy, and sub it parent from self,
  // then add to vrp. This is perform when object is rotated, then vrp is moved
  // too to look into the previous position on the object 

  auto obj2cam_orig = p.vrp_orig_ - p.obj_.world_pos_; 
  auto obj2cam_rot = obj2cam_orig;

  coords::RotatePitch(obj2cam_rot, p.dir_.x, p.trig_);
  coords::RotateYaw(obj2cam_rot, p.dir_.y, p.trig_);
  coords::RotateRoll(obj2cam_rot, p.dir_.z, p.trig_);
  auto obj2cam_new = obj2cam_rot - obj2cam_orig;
  
  cam.vrp_ = p.vrp_orig_ + obj2cam_new;
}

}  // namespace player

}  // namespace anshub