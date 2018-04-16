// *************************************************************
// File:    gl_lights.h
// Descr:   represents lights container with some features
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
#include "lib/math/vector.h"

namespace anshub {

//****************************************************************************
// General lighting structure
//****************************************************************************

struct Lights
{
  void AddAmbient(cFColor&, float intense);
  void AddInfinite(cFColor&, float intense, cVector& dir);
  void AddPoint(cFColor&, float intense, cVector& pos, cVector& dir);

  std::vector<LightAmbient>   ambient_;
  std::vector<LightInfinite>  infinite_;
  std::vector<LightPoint>     point_;

}; // struct Lights

//****************************************************************************
// Helpers
//****************************************************************************

namespace light {
  
  void Reset(Lights&);
  void World2Camera(Lights&, const GlCamera&, const TrigTable&);

  // General lighting functions

  void Object(GlObject&, Lights&);
  void Objects(V_GlObject&, Lights&);
  void Triangles(V_Triangle&, Lights&);

} // namespace light
  
//****************************************************************************
// Implementation of inline member functions
//****************************************************************************

inline void Lights::AddAmbient(cFColor& c, float intense)
{
  ambient_.emplace_back(c, intense);
}

inline void Lights::AddInfinite(cFColor& c, float intense, cVector& dir)
{
  infinite_.emplace_back(c, intense, dir);
}

inline void Lights::AddPoint(cFColor& c, float intense, cVector& pos, cVector& dir)
{
  point_.emplace_back(c, intense, pos, dir);
}

} // namespace anshub

#endif  // GL_LIGHTS_H