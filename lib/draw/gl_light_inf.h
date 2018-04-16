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
#include "gl_camera.h"
#include "gl_coords.h"
#include "lib/math/vector.h"

namespace anshub {

// Lighting structure represents infinite light source

struct LightInfinite
{
  LightInfinite(cFColor& c, float i, cVector& dir);
  LightInfinite(cFColor&& c, float i, cVector&& dir);

  void    Reset() { direction_ = direction_copy_; };
  void    World2Camera(const GlCamera&, const TrigTable&);
  FColor  Illuminate(cFColor& base_color, cVector& normal);
  auto    GetDirection() const { return direction_; }  
  void    SetDirection(cVector&);

  FColor  color_;
  float   intense_;

private:
  Vector  direction_;       // direction of light source
  Vector  direction_copy_;  // used to recover direction after frame 

}; // struct LightInfinite

}  // namespace anshub

#endif  // GL_LIGHT_INFINITE_H