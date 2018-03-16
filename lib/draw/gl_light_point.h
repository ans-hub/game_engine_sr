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
#include "gl_camera.h"
#include "gl_coords.h"
#include "../math/vector.h"

namespace anshub {

struct LightPoint
{
  LightPoint(cFColor& c, float i, cVector& pos, cVector& dir);
  LightPoint(cFColor&& c, float i, cVector&& pos, cVector& dir);

  void    Reset();
  void    World2Camera(const GlCamera&);
  FColor  Illuminate(cFColor& base_color, cVector& normal, cVector& dest);

private:
  FColor  color_;
  float   intense_;
  Vector  position_;          // direction of light source
  Vector  position_copy_;     // used to recover direction after frame 
  Vector  direction_;         // direction of light source
  Vector  direction_copy_;    // used to recover direction after frame 
  float   kc_;
  float   kl_;
  float   kq_;

}; // struct LightPoint

}  // namespace anshub

#endif  // GL_LIGHT_POINT_H