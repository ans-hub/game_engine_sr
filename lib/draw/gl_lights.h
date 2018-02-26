// *************************************************************
// File:    gl_lights.h
// Descr:   represents light
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef GL_LIGHTS_H
#define GL_LIGHTS_H

#include "fx_colors.h"
#include "gl_aliases.h"
#include "gl_object.h"
#include "gl_triangle.h"
#include "gl_light_amb.h"
#include "gl_light_inf.h"
#include "gl_light_point.h"
#include "gl_light_spot.h"
#include "../math/vector.h"

namespace anshub {

// General lighting structure, in the fact - simple container
// of light sources

struct Lights
{
  std::vector<LightAmbient>   ambient_;
  std::vector<LightInfinite>  infinite_;
  std::vector<LightPoint>     point_;
  std::vector<LightSpot>      spot_;

}; // struct Lights

namespace light {
  
  // General lighting functions

  void Object(GlObject&, Lights&);
  void Objects(GlObjects&, Lights&);

} // namespace light
  
} // namespace anshub

#endif  // GL_LIGHTS_H