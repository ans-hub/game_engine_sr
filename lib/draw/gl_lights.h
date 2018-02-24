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
#include "../math/vector.h"
#include "gl_light_amb.h"
#include "gl_light_inf.h"
#include "gl_light_point.h"
#include "gl_light_spot.h"

namespace anshub {

// General lighting structure, in the fact - simple container
// of light sources

struct Lights
{
  LightsAmbient   ambient_;
  LightsInfinite  infinite_;
  LightsPoint     point_;
  LightsSpot      spot_;

}; // struct Lights

namespace light {
  
  // General lighting functions

  void Object(GlObject&, Lights&);
  void Objects(GlObjects&, Lights&);
  void Emission(Triangle&, FColors&);
  void Ambient(Triangle&, cFColor&, cFColor&, cFColor&, Lights&);

  // Specific shading functions for gourang shading

  namespace gourang {

    void Infinite(Triangle&, Vectors&, cFColor&, cFColor&, cFColor&, Lights&);
    void Point(Triangle&, Vectors&, cFColor&, cFColor&, cFColor&, Lights&);

  } // namespace gourang

  // Specific shading functions for flat shading

  namespace flat {

    void Infinite(Triangle&, cFColor&, cFColor&, cFColor&, Lights&);
    void Point(Triangle&, cFColor&, cFColor&, cFColor&, Lights&);
    
  } // namespace flat


} // namespace light
  
}  // namespace anshub

#endif  // GL_LIGHTS_H