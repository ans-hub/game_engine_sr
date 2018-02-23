//***********************************************************************
// File:    gl_triangle.cc
// Descr:   triangle (drawable) struct for renderer
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
//***********************************************************************

#include "gl_triangle.h"

namespace anshub {

// Constructs triangle just with indicies && attributes

Triangle::Triangle(Vertexes& vxs, FColors& c, int f1, int f2, int f3, uint attrs)
  : vxs_{vxs}
  , colors_{c}
  , v1_{vxs[f1]} 
  , v2_{vxs[f2]} 
  , v3_{vxs[f3]} 
  , c1_{c[f1]} 
  , c2_{c[f2]} 
  , c3_{c[f3]} 
  , f1_{f1}
  , f2_{f2}
  , f3_{f3}
  , face_normal_{}
  , attrs_{attrs}
{
  // Calculate face normal

  // Vector u {v1_, v2_};
  // Vector v {v1_, v3_};
  // face_normal_ = vector::CrossProduct(u, v);
}

void triangle::CalcFaceNormal(Triangle& tri, const Vertexes& vxs)
{
  auto p1 = vxs[tri.f1_];
  auto p2 = vxs[tri.f2_];
  auto p3 = vxs[tri.f3_];
  Vector u {p1, p2};
  Vector v {p1, p3};
  tri.face_normal_ = vector::CrossProduct(u, v);
}

}  // namespace anshub