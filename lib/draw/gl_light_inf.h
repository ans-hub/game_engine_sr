// *************************************************************
// File:    gl_light_inf.h
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
  LightInfinite(cFColor& c, float i, cVector& dir);
  LightInfinite(cFColor&& c, float i, cVector&& dir);

  FColor Illuminate(cFColor& base_color, cVector& normal);

private:
  FColor  color_;
  float   intense_;
  Vector  direction_;

}; // struct LightInfinite

}  // namespace anshub

#endif  // GL_LIGHT_INFINITE_H