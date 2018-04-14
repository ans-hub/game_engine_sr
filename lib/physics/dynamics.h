// *************************************************************
// File:    dynamics.h
// Descr:   represent movements using dynamic`s law
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef GC_DYNAMIC_H
#define GC_DYNAMIC_H

#include "lib/math/vector.h"

namespace anshub {

// Performs dynamic movements for different objects such as cameras, objects

struct Dynamics
{
  Dynamics();
  Dynamics(float accel, float frict, float grav, float max_speed);

  auto GetCurrentSpeed() const { return vel_.SquareLength(); }
  auto GetMaxSpeed() const { return max_speed_; }

  // Movements based on current view vector

  void MoveForward(cVector& direction);
  void MoveBackward(cVector& direction);
  void StrafeLeft(cVector& direction);
  void StrafeRight(cVector& direction);

  // Movements based on the sides of the world

  void MoveNorth();
  void MoveSouth();
  void MoveWest();
  void MoveEast();
  void MoveUp();
  void MoveDown();
  void Jump(float height);

  // Rotations

  void RotateYaw(float theta);
  void RotatePitch(float theta);
  void RotateRoll(float theta);

  // Other features

  void SpeedUp(float factor) { accel_factor_ *= factor + math::kEpsilon; }
  void SpeedDown(float factor) { accel_factor_ /= factor + math::kEpsilon; }

  // Velocity functions

  void ProcessVelocity(bool fly_mode, bool on_ground);
  void ProcessDirVelocity();
  auto GetVelocity() const { return vel_; }
  auto GetDirVelocity() const { return dir_vel_; }
  void ResetVelocity() { vel_.Zero(); }
  void ResetAcceleration() { accel_.Zero(); }
  void ResetGravity() { vel_.y = 0.0f; accel_.y = 0.0f; }

  // todo: push_back(move_type); ProcessVelocity(moves); and most of is private

private:

  Vector    vel_;
  Vector    accel_;
  Vector    dir_vel_;
  Vector    dir_accel_;
  float     accel_factor_;
  float     frict_factor_;
  float     gravity_factor_;
  float     max_speed_;
  TrigTable trig_;

}; // struct Dynamics

}  // namespace anshub

#endif  // GC_DYNAMIC_H