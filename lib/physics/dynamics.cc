// *************************************************************
// File:    dynamics.cc
// Descr:   represent movements using dynamic`s law
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#include "dynamics.h"

namespace anshub {

Dynamics::Dynamics()
  : vel_{}
  , accel_{}
  , dir_vel_{}
  , dir_accel_{}
  , accel_factor_{}
  , frict_factor_{}
  , gravity_factor_{}
  , max_speed_{}
  , trig_{}
{ }

Dynamics::Dynamics(
  float accel_factor,
  float frict_factor,
  float grav_factor,
  float max_speed
)
  : vel_{0.0f, 0.0f, 0.0f}
  , accel_{0.0f, 0.0f, 0.0f}
  , dir_vel_{0.0f, 0.0f, 0.0f}
  , dir_accel_{0.0f, 0.0f, 0.0f}
  , accel_factor_{accel_factor}
  , frict_factor_{frict_factor}
  , gravity_factor_{grav_factor}
  , max_speed_{max_speed}
  , trig_{}
{ }

void Dynamics::MoveForward(cVector& dir)
{
  accel_.z += accel_factor_ * trig_.Cos(dir.y);
  accel_.x += accel_factor_ * trig_.Sin(dir.y);
}

void Dynamics::MoveBackward(cVector& dir)
{
  accel_.z -= accel_factor_ * trig_.Cos(dir.y);
  accel_.x -= accel_factor_ * trig_.Sin(dir.y);
}

void Dynamics::StrafeLeft(cVector& dir)
{
  accel_.x -= accel_factor_ * trig_.Cos(dir.y);
  accel_.z += accel_factor_ * trig_.Sin(dir.y);
}

void Dynamics::StrafeRight(cVector& dir)
{   
  accel_.x += accel_factor_ * trig_.Cos(dir.y);
  accel_.z -= accel_factor_ * trig_.Sin(dir.y);
}

void Dynamics::MoveWest()
{
  accel_.x -= accel_factor_;
}

void Dynamics::MoveEast()
{
  accel_.x += accel_factor_;
}

void Dynamics::MoveNorth()
{
  accel_.z += accel_factor_;
}

void Dynamics::MoveSouth()
{
  accel_.z -= accel_factor_;
}

void Dynamics::MoveUp()
{
  accel_.y += accel_factor_;
}

void Dynamics::MoveDown()
{
  accel_.y -= accel_factor_;
}

void Dynamics::Jump(float height)
{
  vel_.y = height;
}

void Dynamics::RotateYaw(float theta)
{
  dir_accel_.y -= theta * accel_factor_;
}

void Dynamics::RotatePitch(float theta)
{
  dir_accel_.x -= theta * accel_factor_;  
}

void Dynamics::RotateRoll(float theta)
{
  dir_accel_.z -= theta * accel_factor_;  
}

// Apply move velocity in each frame after all movements

void Dynamics::ProcessVelocity(bool fly_mode, bool on_ground)
{
  if (!on_ground || !vel_.IsZero() || !accel_.IsZero())
  {
    // When in fly mode, we apply all velocity vector

    if (fly_mode)
    {
      accel_ *= frict_factor_;    // we should dec accel
      vel_ += accel_;
      if (vel_.SquareLength() > max_speed_)
        vel_ -= accel_;
      vel_ *= frict_factor_;      // and dec vel
    }

    // But if not in fly mode, we apply gravity and friction separately

    else {
      accel_.x *= frict_factor_;
      accel_.z *= frict_factor_;
      vel_ += accel_;
      if (vel_.SquareLength() > max_speed_)
        vel_ -= accel_;
      vel_.x *= frict_factor_;
      vel_.z *= frict_factor_;

      if (!on_ground)
        vel_.y += gravity_factor_;     // process gravity
    }
  }
  else 
  {
    vel_.Zero();
    accel_.Zero();
  }
}

// Apply direction velocity in each frame after all movements

void Dynamics::ProcessDirVelocity()
{
  if (!dir_vel_.IsZero() || !dir_accel_.IsZero())
  {
    dir_accel_ *= frict_factor_;    // we should dec accel
    dir_vel_ += dir_accel_;
    dir_vel_ *= frict_factor_;      // and dec vel
  }
  else 
  {
    dir_vel_.Zero();
    dir_accel_.Zero();
  }
}

} // namespace anshub