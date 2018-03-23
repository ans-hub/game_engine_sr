//***********************************************************************
// File:    gl_face.h
// Descr:   drawable face struct for object
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
//***********************************************************************

#ifndef GC_GL_FACE_H
#define GC_GL_FACE_H

#include "gl_aliases.h"
#include "fx_colors.h"
#include "gl_vertex.h"
#include "../math/vector.h"

namespace anshub {

//**********************************************************************
// Represent face of drawable GlObject
//**********************************************************************

struct Face
{
  Face(V_Vertex& vxs, int f1, int f2, int f3) 
  : active_{true}
  , vxs_{f1, f2, f3}
  , normal_{
      vector::CrossProduct(
        Vector{vxs[f2].pos_-vxs[f1].pos_}, Vector{vxs[f3].pos_-vxs[f1].pos_}) }
  , color_{}
  , angles_{
    vector::AngleBetween(
      Vector(vxs[f2].pos_ - vxs[f1].pos_), Vector(vxs[f3].pos_ - vxs[f1].pos_)
    ),
    vector::AngleBetween(
      Vector(vxs[f1].pos_ - vxs[f2].pos_), Vector(vxs[f3].pos_ - vxs[f2].pos_)
    ),
    vector::AngleBetween(
      Vector(vxs[f1].pos_ - vxs[f3].pos_), Vector(vxs[f2].pos_ - vxs[f3].pos_)
    ) }
  {
    if (!normal_.IsZero())
      normal_.Normalize();
  }

  int& operator[](int f) { return vxs_[f]; }
  const int& operator[](int f) const { return vxs_[f]; }

  bool      active_;
  A3_Int    vxs_;     // numbers of vertices in vertices list
  Vector    normal_;  // face normal
  FColor    color_;   // face color
  A3_Float  angles_;  // angles to compute vertices normals

}; // struct Face

} // namespace anshub

#endif  // GC_GL_FACE_H