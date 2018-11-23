// ******************************************************************
// File:    gl_lights.h
// Descr:   represents container of light sources with some features
// Author:  Novoselov Anton @ 2017
// ******************************************************************

#ifndef GL_LIGHTS_H
#define GL_LIGHTS_H

#include "lights/ambient.h"
#include "lights/infinite.h"
#include "lights/point.h"

#include "lib/render/fx_colors.h"
#include "lib/render/gl_aliases.h"
#include "lib/render/gl_object.h"
#include "lib/render/gl_triangle.h"

#include "lib/math/vector.h"

namespace anshub {

//****************************************************************************
// Container of light sources
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