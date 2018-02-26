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
{ 
  math::Clamp(intense_, 0.0f, 1.0f);
  kc_ = 1.0f;
  kl_ = 1.0f;
  kq_ = 0.0f;
}

LightPoint::LightPoint(cFColor&& c, float i, cVector&& pos, cVector& dir)
  : color_{c}
  , intense_{i}
  , position_{pos}
  , direction_{dir}
{
  math::Clamp(intense_, 0.0f, 1.0f);  
  kc_ = 1.0f;
  kl_ = 1.0f;
  kq_ = 0.0f;
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

}  // namespace anshub