//***********************************************************************
// File:    gl_triangle.cc
// Descr:   triangle (drawable) struct for renderer
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
//***********************************************************************

#include "gl_triangle.h"

namespace anshub {

// Constructs triangle just with indicies && attributes

Triangle::Triangle(int v1, int v2, int v3, uint attrs)
  : vxs_{}
  , colors_{}
  , indicies_{v1, v2, v3}
  , attrs_{attrs}
{ }

// Constructs triangle with self contained vertexes and colors

Triangle::Triangle(
  cVector& v1, cVector& v2, cVector& v3, cColor& c1, cColor& c2, cColor& c3,
  uint attrs)
  : vxs_{v1, v2, v3}
  , colors_{c1, c2, c3}
  , indicies_{}
  , attrs_{attrs}
{ }


}  // namespace anshub