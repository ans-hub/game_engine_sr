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
#include "../math/vector.h"

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
  , color_{r, g, b, 0.0f} { }

  Vector  pos_;
  Vector  normal_;
  FColor  color_;
  Vector  texture_;
  
}; // struct Vertex

} // namespace anshub

#endif  // GC_GL_VERTEX_H