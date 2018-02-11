//***********************************************************************
// File:    gl_triangle.cc
// Descr:   triangle (drawable) struct for renderer
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
//***********************************************************************

#include "gl_triangle.h"

namespace anshub {

// Constructs triangle with given vxs ans edges (reference to vxs)

Triangle::Triangle(
  Vertexes& vxs, int e1, int e2, int e3, uint color, uint attrs)
  : vxs_{}
  , indicies_{e1, e2, e3}
  , color_{color}
  , attrs_{attrs}
{ }

// Constructs triangle with given vxs ans edges (copy)

Triangle::Triangle(
  const Vector& p1, const Vector& p2, const Vector& p3, uint color, uint attrs)
  : vxs_{p1, p2, p3}
  , indicies_{0, 1, 2}
  , color_{color}
  , attrs_{attrs}
{ }

}  // namespace anshub