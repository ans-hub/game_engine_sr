// *************************************************************
// File:    physics.cc
// Descr:   represent simple physics
// Author:  Novoselov Anton @ 2017
// *************************************************************

#include "physics.h"

namespace anshub {

Physics::Physics()
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

Physics::Physics(
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

void Physics::MoveForward(cVector& dir)
{
  accel_.z += accel_factor_ * trig_.Cos(dir.y);
  accel_.x += accel_factor_ * trig_.Sin(dir.y);
}

void Physics::MoveBackward(cVector& dir)
{
  accel_.z -= accel_factor_ * trig_.Cos(dir.y);
  accel_.x -= accel_factor_ * trig_.Sin(dir.y);
}

void Physics::StrafeLeft(cVector& dir)
{
  accel_.x -= accel_factor_ * trig_.Cos(dir.y);
  accel_.z += accel_factor_ * trig_.Sin(dir.y);
}

void Physics::StrafeRight(cVector& dir)
{   
  accel_.x += accel_factor_ * trig_.Cos(dir.y);
  accel_.z -= accel_factor_ * trig_.Sin(dir.y);
}

void Physics::MoveWest()
{
  accel_.x -= accel_factor_;
}

void Physics::MoveEast()
{
  accel_.x += accel_factor_;
}

void Physics::MoveNorth()
{
  accel_.z += accel_factor_;
}

void Physics::MoveSouth()
{
  accel_.z -= accel_factor_;
}

void Physics::MoveUp()
{
  accel_.y += accel_factor_;
}

void Physics::MoveDown()
{
  accel_.y -= accel_factor_;
}

void Physics::Jump(float height)
{
  vel_.y = height;
}

void Physics::RotateYaw(float theta)
{
  dir_accel_.y -= theta * accel_factor_;
}

void Physics::RotatePitch(float theta)
{
  dir_accel_.x -= theta * accel_factor_;  
}

void Physics::RotateRoll(float theta)
{
  dir_accel_.z -= theta * accel_factor_;  
}

void Physics::ProcessVelocity(bool fly_mode, bool on_ground)
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

void Physics::ProcessDirVelocity()
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