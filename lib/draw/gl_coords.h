// *****************************************************************
// File:    gl_coords.h
// Descr:   functions to convert different coordinates (non-marices)
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *****************************************************************

#ifndef GC_GL_COORDS_H
#define GC_GL_COORDS_H

#include "gl_aliases.h"
#include "gl_vertex.h"
#include "lib/math/trig.h"
#include "lib/math/vector.h"
#include "lib/math/matrix_rotate_uvn.h"

namespace anshub {

namespace coords {

  void  Local2World(V_Vertex&, const Vector& move);
  void  Camera2Persp(V_Vertex&, float dov, float ar);
  void  World2Camera(V_Vertex&, cVector& pos, cVector& dir, const TrigTable&);
  void  World2Camera(Vector&, cVector& pos, cVector& dir, const TrigTable&);
  void  Persp2Screen(V_Vertex&, float wov, int scr_w, int scr_h);

  void  RotateYaw(V_Vertex&, float deg, TrigTable&);
  void  RotatePitch(V_Vertex&, float deg, TrigTable&);
  void  RotateRoll(V_Vertex&, float deg, TrigTable&);

  void  RotateYaw(Vector&, float deg, const TrigTable&);
  void  RotatePitch(Vector&, float deg, const TrigTable&);
  void  RotateRoll(Vector&, float deg, const TrigTable&);

  Vector RotationMatrix2Euler(const MatrixRotateUvn&);
  
} // namespace coords

} // namespace anshub

#endif  // GC_GL_OBJECT_H