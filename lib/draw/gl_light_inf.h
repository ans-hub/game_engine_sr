// *************************************************************
// File:    gl_light)inf.h
// Descr:   represents infinite light source
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef GL_LIGHT_INFINITE_H
#define GL_LIGHT_INFINITE_H

#include "fx_colors.h"
#include "gl_aliases.h"
#include "../math/vector.h"

namespace anshub {

// Lighting structure represents infinite light source

struct LightInfinite
{
  LightInfinite(cFColor& c, float i, cVector& dir)
  : color_{c}
  , intense_{i}
  , direction_{dir}
  { 
    direction_.Normalize();
    math::Clamp(intense_, 0.0f, 1.0f);  
  }
  LightInfinite(cFColor&& c, float i, cVector&& dir)
  : color_{c}
  , intense_{i}
  , direction_{dir}
  { 
    direction_.Normalize();
    math::Clamp(intense_, 0.0f, 1.0f);  
  }
  
  FColor  color_;
  float   intense_;
  Vector  direction_;

}; // struct LightInfinite

}  // namespace anshub

#endif  // GL_LIGHT_INFINITE_H