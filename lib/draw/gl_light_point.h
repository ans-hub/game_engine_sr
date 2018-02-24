// *************************************************************
// File:    gl_light_point.h
// Descr:   represents point light source
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef GL_LIGHT_POINT_H
#define GL_LIGHT_POINT_H

#include "fx_colors.h"
#include "gl_aliases.h"
#include "../math/vector.h"

namespace anshub {

struct LightPoint
{
  LightPoint(cFColor& c, float i, cVector& pos)
  : color_{c}
  , intense_{i}
  , position_{pos}
  { 
    math::Clamp(intense_, 0.0f, 1.0f);  
  }
  LightPoint(cFColor&& c, float i, cVector&& pos)
  : color_{c}
  , intense_{i}
  , position_{pos}
  { 
    math::Clamp(intense_, 0.0f, 1.0f);  
  }
  
  FColor  color_;
  float   intense_;
  Vector  position_;
  float   ka;
  float   kb;
  float   kc;

}; // struct LightPoint

}  // namespace anshub

#endif  // GL_LIGHT_POINT_H