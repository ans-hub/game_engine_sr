// *************************************************************
// File:    gl_light_amb.h
// Descr:   represents ambient light source
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef GL_LIGHT_AMBIENT_H
#define GL_LIGHT_AMBIENT_H

#include "lib/draw/fx_colors.h"
#include "lib/draw/gl_aliases.h"

namespace anshub {

// Lighting structure represents ambient lighting of the scene

struct LightAmbient
{
  LightAmbient(cFColor& c, float i);
  LightAmbient(cFColor&& c, float i);
  FColor Illuminate(cFColor& base_color);

  FColor  color_;
  float   intense_; 

}; // struct LightAmbient

// Implementation of member functions

inline LightAmbient::LightAmbient(cFColor& c, float i)
  : color_{c}
  , intense_{i}
{
  math::Clamp(intense_, 0.0f, 1.0f);      
}

inline LightAmbient::LightAmbient(cFColor&& c, float i)
  : LightAmbient(c, i)
{ }

inline FColor LightAmbient::Illuminate(cFColor& base_color)
{
  return (base_color * color_ * intense_) / 256.0f;
}

}  // namespace anshub

#endif  // GL_LIGHT_AMBIENT_H