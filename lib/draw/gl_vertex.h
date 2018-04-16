//***********************************************************************
// File:    gl_vertex.h
// Descr:   drawable vertex
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
//***********************************************************************

#ifndef GC_GL_VERTEX_H
#define GC_GL_VERTEX_H

#include "gl_aliases.h"
#include "fx_colors.h"
#include "lib/math/vector.h"

namespace anshub {

//**********************************************************************
// Represent drawable vertex
//**********************************************************************

struct Vertex
{
  Vertex()
  : pos_{}
  , normal_{}
  , color_{}
  , texture_{} { }
  explicit Vertex(const Vector& v)
  : pos_{v}
  , normal_{}
  , color_{}
  , texture_{} { }
  Vertex(float x, float y, float z, float r, float g, float b)
  : pos_{x, y, z}
  , normal_{}
  , color_{r, g, b} { }

  Vector  pos_;
  Vector  normal_;
  FColor  color_;
  Vector  texture_;
  
}; // struct Vertex

}  // namespace anshub

#endif  // GC_GL_VERTEX_H

// Note : it is more logical to do Vertex as derived class from Vector, since
// vertex in the first is defined by its position, but I am afraid that this
// will slow down system (we copy vertices every frame, and in this case
// we would have one more copy constructor calling (for base class))