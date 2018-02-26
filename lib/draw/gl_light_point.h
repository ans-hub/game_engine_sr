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
  LightPoint(cFColor& c, float i, cVector& pos, cVector& dir);
  LightPoint(cFColor&& c, float i, cVector&& pos, cVector& dir);

  FColor Illuminate(cFColor& base_color, cVector& normal, cVector& dest);

private:
  FColor  color_;
  float   intense_;
  Vector  position_;
  Vector  direction_;
  float   kc_;
  float   kl_;
  float   kq_;

}; // struct LightPoint

}  // namespace anshub

#endif  // GL_LIGHT_POINT_H