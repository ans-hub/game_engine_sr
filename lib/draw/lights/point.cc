// *************************************************************
// File:    point.cc
// Descr:   represents point light source
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#include "point.h"

namespace anshub {

LightPoint::LightPoint(cFColor& color, float intense, cVector& pos, cVector& dir)
  : LightSource{color, intense}
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
}

LightPoint::LightPoint(
  cFColor& color, float intense, cVector& pos, cVector& dir,
  float kc, float kl, float kq)
  : LightPoint{color, intense, pos, dir}
{
  kc_ = kc;
  kl_ = kl;
  kq_ = kq;
}

void LightPoint::Reset()
{
  position_ = position_copy_;
  direction_ = direction_copy_;
}

void LightPoint::World2Camera(const GlCamera& cam, const TrigTable& trig)
{
  // Just rotate direction vector

  coords::RotateYaw(direction_, -cam.dir_.y, trig);
  coords::RotatePitch(direction_, -cam.dir_.x, trig);
  coords::RotateRoll(direction_, -cam.dir_.z, trig);
  direction_.Normalize();

  // Move and rotate position vector

  position_ += cam.vrp_ * (-1.0f);
  coords::RotateYaw(position_, -cam.dir_.y, trig);
  coords::RotatePitch(position_, -cam.dir_.x, trig);
  coords::RotateRoll(position_, -cam.dir_.z, trig);
}

FColor LightPoint::Illuminate() const
{
  auto dir = direction_ * (-1);
  auto prod = vector::DotProduct(dir, *args_.normal_);
  if (prod < 0) prod = 0;

  auto distance = position_ - *args_.destination_;
  auto len = distance.Length();
  auto k = (color_ * intense_) / (kc_ + kl_ * len + kq_ * len * len);

  return (args_.base_color_ * prod * k) / 256.0f;
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