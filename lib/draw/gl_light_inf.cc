// *************************************************************
// File:    gl_light_inf.cc
// Descr:   represents infinite light source
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#include "gl_light_inf.h"

namespace anshub {

LightInfinite::LightInfinite(cFColor& c, float i, cVector& dir)
  : color_{c}
  , intense_{i}
  , direction_{dir}
  , direction_copy_{}
{ 
  direction_.Normalize();
  direction_copy_ = direction_;  
  math::Clamp(intense_, 0.0f, 1.0f);  
}

LightInfinite::LightInfinite(cFColor&& c, float i, cVector&& dir)
  : LightInfinite(c, i, dir) { }

void LightInfinite::World2Camera(const GlCamera& cam, const TrigTable& trig)
{
  // Just rotate direction vector
  
  coords::RotateYaw(direction_, -cam.dir_.y, trig);
  coords::RotatePitch(direction_, -cam.dir_.x, trig);
  coords::RotateRoll(direction_, -cam.dir_.z, trig);
  direction_.Normalize();
}

FColor LightInfinite::Illuminate(cFColor& base_color, cVector& normal)
{
  auto dir = direction_ * (-1.0f);
  auto prod = vector::DotProduct(dir, normal);
  if (prod < 0) prod = 0;
  return (base_color * color_ * intense_ * prod) / 256.0f;
}

void LightInfinite::SetDirection(cVector& dir)
{
  direction_ = dir;
  direction_.Normalize();
  direction_copy_ = direction_;
}

}  // namespace anshub