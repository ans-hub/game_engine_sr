//***********************************************************************
// File:    gl_vertex.h
// Descr:   drawable vertex
// Author:  Novoselov Anton @ 2017
//***********************************************************************

// todo: Vertex is simple Vector<3>

#ifndef GC_GL_VERTEX_H
#define GC_GL_VERTEX_H

#include "lib/render/gl_aliases.h"
#include "lib/render/fx_colors.h"

#include "lib/math/vector.h"

namespace anshub {

struct Vertex
{
  Vertex();
  explicit Vertex(const Vector&);
  Vertex(float x, float y, float z, float r, float g, float b);

  Vector  pos_;
  Vector  normal_;
  FColor  color_;
  Vector  texture_;
  
}; // struct Vertex

inline Vertex::Vertex()
  : pos_{}
  , normal_{}
  , color_{}
  , texture_{}
{ }

inline Vertex::Vertex(const Vector& v)
  : pos_{v}
  , normal_{}
  , color_{}
  , texture_{}
{ }

inline Vertex::Vertex(float x, float y, float z, float r, float g, float b)
  : pos_{x, y, z}
  , normal_{}
  , color_{r, g, b}
{ }

}  // namespace anshub

#endif  // GC_GL_VERTEX_H
