// *************************************************************
// File:    simple.h
// Descr:   represent simple physics
// Author:  Novoselov Anton @ 2017
// *************************************************************

#ifndef GC_PHYSICS_H
#define GC_PHYSICS_H

#include "lib/math/vector.h"

namespace anshub {

struct Physics
{
  Physics();
  Physics(float accel, float frict, float grav, float max_speed);

  auto GetCurrentSpeed() const { return vel_.SquareLength(); }
  auto GetMaxSpeed() const { return max_speed_; }

  void MoveForward(cVector& direction);
  void MoveBackward(cVector& direction);
  void StrafeLeft(cVector& direction);
  void StrafeRight(cVector& direction);

  void MoveNorth();
  void MoveSouth();
  void MoveWest();
  void MoveEast();
  void MoveUp();
  void MoveDown();
  void Jump(float height);

  void RotateYaw(float theta);
  void RotatePitch(float theta);
  void RotateRoll(float theta);

  void SpeedUp(float factor) { accel_factor_ *= (factor + math::kEpsilon); }
  void SpeedDown(float factor) { accel_factor_ /= (factor + math::kEpsilon); }

  void ProcessVelocity(bool fly_mode, bool on_ground);
  void ProcessDirVelocity();
  void ResetVelocity() { vel_.Zero(); }
  void ResetAcceleration() { accel_.Zero(); }
  void ResetGravity() { vel_.y = 0.0f; accel_.y = 0.0f; }
  
  auto GetVelocity() const { return vel_; }
  auto GetAcceleration() const { return accel_; }
  auto GetDirAcceleration() const { return dir_accel_; }
  auto GetDirVelocity() const { return dir_vel_; }

  void SetVelocity(const Vector& v) { vel_ = v; } 
  void SetAcceleration(const Vector& v) { accel_ = v; } 

private:

  Vector      vel_;
  Vector      accel_;
  Vector      dir_vel_;
  Vector      dir_accel_;
  float       accel_factor_;
  float       frict_factor_;
  float       gravity_factor_;
  float       max_speed_;
  TrigTable   trig_;

}; // struct Physics

}  // namespace anshub

#endif  // GC_PHYSICS_H