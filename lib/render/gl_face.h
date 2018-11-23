//***********************************************************************
// File:    gl_face.h
// Descr:   face struct for object
// Author:  Novoselov Anton @ 2017
//***********************************************************************

#ifndef GC_GL_FACE_H
#define GC_GL_FACE_H

#include "lib/render/gl_aliases.h"
#include "lib/render/fx_colors.h"
#include "lib/render/gl_vertex.h"

#include "lib/math/vector.h"

namespace anshub {

struct Face
{
  Face(V_Vertex& vxs, int f1, int f2, int f3);

  int& operator[](int f) { return vxs_[f]; }
  const int& operator[](int f) const { return vxs_[f]; }

  bool      active_;
  bool      double_sided_;
  A3_Int    vxs_;     // numbers of vertices in vertices list
  Vector    normal_;
  FColor    color_;
  A3_Float  angles_;  // angles to compute vertices normals

}; // struct Face

//**********************************************************************
// Inline implementation
//**********************************************************************

inline Face::Face(V_Vertex& vxs, int f1, int f2, int f3) 
  : active_{true}
  , double_sided_{false}
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

} // namespace anshub

#endif  // GC_GL_FACE_H