// *****************************************************************
// File:    gl_coords.h
// Descr:   functions to convert different coordinates (non-marices)
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *****************************************************************

#ifndef GC_GL_COORDS_H
#define GC_GL_COORDS_H

#include "gl_aliases.h"
#include "../math/trig.h"
#include "../math/vector.h"
#include "../math/matrix_rotate_uvn.h"

namespace anshub {

namespace coords {

  void  Local2World(Vertexes&, const Vector& move);
  void  Camera2Persp(Vertexes&, float dov, float ar);
  void  World2Camera(Vertexes&, cVector& pos, cVector& dir, const TrigTable&);
  void  Persp2Screen(Vertexes&, float wov, int scr_w, int scr_h);

  void  RotateYaw(Vertexes&, float deg, TrigTable&);
  void  RotatePitch(Vertexes&, float deg, TrigTable&);
  void  RotateRoll(Vertexes&, float deg, TrigTable&);

  Vector RotationMatrix2Euler(const MatrixRotateUvn&);
  
} // namespace coords

} // namespace anshub

#endif  // GC_GL_OBJECT_H