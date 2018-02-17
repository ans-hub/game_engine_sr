// *****************************************************************
// File:    gl_coords.h
// Descr:   functions to convert different coordinates (non-marices)
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *****************************************************************

#ifndef GC_GL_COORDS_H
#define GC_GL_COORDS_H

#include "gl_object.h"
#include "gl_aliases.h"
#include "../math/trig.h"
#include "../math/vector.h"
#include "../math/matrix_rotate_uvn.h"

namespace anshub {

namespace coords {

  void  Local2World(Vertexes&, const Vector& move);
  void  World2Camera(Vertexes&);
  void  Camera2Persp(Vertexes&, float dov, float ar);
  void  World2Camera(Vertexes&, Vector& pos, Vector& dir, TrigTable&);
  void  Persp2Screen(Vertexes&, float wov, int scr_w, int scr_h);

  void  RotateYaw(Vertexes&, float deg, TrigTable&);
  void  RotatePitch(Vertexes&, float deg, TrigTable&);
  void  RotateRoll(Vertexes&, float deg, TrigTable&);

  void  Homogenous2Normal(GlObject&);
  void  Homogenous2Normal(std::vector<GlObject>&);
  Vector  RotationMatrix2Euler(const MatrixRotateUvn&);
  
} // namespace coords

//*****************************************************************************
// Inline implementation
//*****************************************************************************

// Converts homogenous coordinates (w != 1) to normal 3d

inline void coords::Homogenous2Normal(GlObject& obj)
{
  for (auto& vx : obj.GetCoords())
    vector::ConvertFromHomogeneous(vx);
}

// The same function as above but for array of objects

inline void coords::Homogenous2Normal(std::vector<GlObject>& arr)
{
  for (auto& obj : arr)
    for (auto& vx : obj.GetCoords())
      vector::ConvertFromHomogeneous(vx);
}

} // namespace anshub

#endif  // GC_GL_OBJECT_H