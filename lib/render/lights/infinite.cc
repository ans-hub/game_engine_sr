// *************************************************************
// File:    infinite.cc
// Descr:   represents infinite light source
// Author:  Novoselov Anton @ 2017
// *************************************************************

#include "infinite.h"

namespace anshub {

LightInfinite::LightInfinite(cFColor& color, float intense, cVector& dir)
  : LightSource{color, intense}
  , direction_{dir}
  , direction_copy_{}
{ 
  direction_.Normalize();
  direction_copy_ = direction_;  
}

void LightInfinite::World2Camera(const GlCamera& cam, const TrigTable& trig)
{
  // Just rotate direction vector
  
  coords::RotateYaw(direction_, -cam.dir_.y, trig);
  coords::RotatePitch(direction_, -cam.dir_.x, trig);
  coords::RotateRoll(direction_, -cam.dir_.z, trig);
  direction_.Normalize();
}

FColor LightInfinite::Illuminate() const
{
  auto dir = direction_ * (-1.0f);
  auto prod = vector::DotProduct(dir, *args_.normal_);
  if (prod < 0) prod = 0;
  return (args_.base_color_ * color_ * intense_ * prod) / 256.0f;
}

void LightInfinite::SetDirection(cVector& dir)
{
  direction_ = dir;
  direction_.Normalize();
  direction_copy_ = direction_;
}

}  // namespace anshub