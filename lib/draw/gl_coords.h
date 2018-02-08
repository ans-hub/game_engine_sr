// *****************************************************************
// File:    gl_coords.h
// Descr:   functions to convert different coordinates (non-marices)
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *****************************************************************

#ifndef GC_GL_COORDS_H
#define GC_GL_COORDS_H

#include "../math/trig.h"
#include "../math/vector.h"

namespace anshub {

using Vertexes    = std::vector<Vector>;

namespace coord {

  Vertexes  Local2World(const Vertexes&, const Vector& move);
  Vertexes  World2Camera(const Vertexes&);
  Vertexes  Camera2Persp(const Vertexes&, float dov, float ar);
  Vertexes  World2Camera(const Vertexes&, Vector& pos, Vector& dir, TrigTable&);
  Vertexes  Persp2Screen(const Vertexes&, float wov, int scr_w, int scr_h);

} // namespace coords

} // namespace anshub

#endif  // GC_GL_OBJECT_H