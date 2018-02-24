// *************************************************************
// File:    gl_light_amb.h
// Descr:   represents ambient light source
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef GL_LIGHT_AMBIENT_H
#define GL_LIGHT_AMBIENT_H

#include "fx_colors.h"
#include "gl_aliases.h"

namespace anshub {

// Lighting structure represents ambient lighting of the scene

struct LightAmbient
{
  LightAmbient(FColor& c, float i)
  : color_{c}
  , intense_{i}
  {
    math::Clamp(intense_, 0.0f, 1.0f);      
  }
  LightAmbient(FColor&& c, float i)
  : color_{c}
  , intense_{i}
  {
    math::Clamp(intense_, 0.0f, 1.0f);      
  }
  FColor  color_;
  float   intense_; 

}; // struct LightAmbient

}  // namespace anshub

#endif  // GL_LIGHT_AMBIENT_H