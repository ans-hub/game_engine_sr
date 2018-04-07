// *************************************************************
// File:    gl_light_point.cc
// Descr:   represents point light source
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#include "gl_light_point.h"

namespace anshub {

LightPoint::LightPoint(cFColor& c, float i, cVector& pos, cVector& dir)
  : color_{c}
  , intense_{i}
  , position_{pos}
  , direction_{dir}
  , position_copy_{}
  , direction_copy_{}
  , kc_{1.0f}
  , kl_{1.0f}
  , kq_{0.0f}
{
  direction_.Normalize();

  direction_copy_ = direction_;
  position_copy_ = position_;
  
  math::Clamp(intense_, 0.0f, 1.0f);
}

LightPoint::LightPoint(cFColor&& c, float i, cVector&& pos, cVector& dir)
  : LightPoint(c, i, pos, dir) { }

void LightPoint::Reset()
{
  position_ = position_copy_;
  direction_ = direction_copy_;
}

void LightPoint::World2Camera(const GlCamera& cam)
{
  // Just rotate direction vector

  coords::RotateYaw(direction_, -cam.dir_.y, cam.trig_);
  coords::RotatePitch(direction_, -cam.dir_.x, cam.trig_);
  coords::RotateRoll(direction_, -cam.dir_.z, cam.trig_);
  direction_.Normalize();

  // Move and rotate position vector

  position_ += cam.vrp_ * (-1.0f);
  coords::RotateYaw(position_, -cam.dir_.y, cam.trig_);
  coords::RotatePitch(position_, -cam.dir_.x, cam.trig_);
  coords::RotateRoll(position_, -cam.dir_.z, cam.trig_);
}

FColor LightPoint::Illuminate(
  cFColor& base_color, cVector& normal, cVector& dest)
{
  auto dir = direction_ * (-1);
  auto prod = vector::DotProduct(dir, normal);
  if (prod < 0) prod = 0;

  auto distance = position_ - dest;
  auto len = distance.Length();
  auto k = (color_ * intense_) / (kc_ + kl_ * len + kq_ * len * len);

  return (base_color * prod * k) / 256.0f;
}

void LightPoint::SetPosition(cVector& pos)
{
  position_ = pos;
  position_copy_ = position_;
}

void LightPoint::SetDirection(cVector& dir)
{ 
  direction_ = dir;
  direction_.Normalize();
  direction_copy_ = direction_;
}

}  // namespace anshub