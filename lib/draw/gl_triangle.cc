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
  , f1_{f1}
  , f2_{f2}
  , f3_{f3}
  , a1_{vector::AngleBetween(Vector(v2_-v1_), Vector(v3_-v1_))}
  , a2_{vector::AngleBetween(Vector(v1_-v2_), Vector(v3_-v2_))}
  , a3_{vector::AngleBetween(Vector(v1_-v3_), Vector(v2_-v3_))}
  , face_normal_{}
  , face_color_{c[f1]}
  , attrs_{attrs}
{

}

}  // namespace anshub