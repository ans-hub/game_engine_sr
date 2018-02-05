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

  Vertexes  Local2World(const Vertexes& vxs, const Vector& move);
  Vertexes  World2Camera(const Vertexes& vxs);
  Vertexes  Camera2Persp(const Vertexes& vxs, float dov, float ar);
  Vertexes  World2Camera(const Vertexes&);
  Vertexes  Persp2Screen(const Vertexes& vxs, float wov, int scr_w, int scr_h);

} // namespace coords

} // namespace anshub

#endif  // GC_GL_OBJECT_H