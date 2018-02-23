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
  : v1_{vxs[f1]} 
  , v2_{vxs[f2]} 
  , v3_{vxs[f3]} 
  , c1_{c[f1]} 
  , c2_{c[f2]} 
  , c3_{c[f3]} 
  // , colors_{}
  , f1_{f1}
  , f2_{f2}
  , f3_{f3}
  // , indicies_{f1, f2, f3}
  , attrs_{attrs}
{ }

// // Constructs triangle with self contained vertexes and colors

// Triangle::Triangle(
//   cVector& v1, cVector& v2, cVector& v3, cColor& c1, cColor& c2, cColor& c3,
//   uint attrs)
//   : v1_{v1}
//   , v2_{v2}
//   , v3_{v3}
//   , c1_{c1}
//   , c2_{c2}
//   , c3_{c3}
//   , colors_{}
//   , indicies_{}
//   , attrs_{attrs}
// { }

}  // namespace anshub