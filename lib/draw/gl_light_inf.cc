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
{ 
  direction_.Normalize();
  math::Clamp(intense_, 0.0f, 1.0f);  
}

LightInfinite::LightInfinite(cFColor&& c, float i, cVector&& dir)
  : color_{c}
  , intense_{i}
  , direction_{dir}
{ 
  direction_.Normalize();
  math::Clamp(intense_, 0.0f, 1.0f);  
}

FColor LightInfinite::Illuminate(cFColor& base_color, cVector& normal)
{
  auto dir = direction_ * (-1);
  auto prod = vector::DotProduct(dir, normal);
  if (prod < 0) prod = 0;
  return (base_color * color_ * intense_ * prod) / 256.0f;
}

}  // namespace anshub