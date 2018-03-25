// *************************************************************
// File:    gl_draw.h
// Descr:   draws objects and triangles arrays
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef GL_DRAW_H
#define GL_DRAW_H

#include <cmath>
#include <algorithm>

#include "fx_colors.h"
#include "gl_buffer.h"
#include "gl_object.h"
#include "gl_vertex.h"
#include "gl_triangle.h"
#include "gl_z_buffer.h"
#include "gl_alpha_lut.h"
#include "fx_rasterizers.h"

#include "../math/segment.h"

namespace anshub {

// Represents struct to hold different information about
// rendering

struct RenderContext
{
  RenderContext(int w, int h, int color)
    : is_wired_{false}
    , is_alpha_{false}
    , is_zbuf_{true}
    , clarity_{}
    , pixels_drawn_{}
    , sbuf_{w, h, color}
    , zbuf_{w, h}
    , alpha_lut_{}
  { }

  // Context members

  bool is_wired_;
  bool is_alpha_;
  bool is_zbuf_;
  int  clarity_;
  int  pixels_drawn_;

  // Context entities

  Buffer    sbuf_;
  ZBuffer   zbuf_;
  AlphaLut  alpha_lut_;

}; // struct RenderContext

// Functions that draws objects

namespace draw_object {

  int  Wired(const GlObject&, Buffer&);
  int  Solid(const GlObject&, Buffer&);
  void Normals(const GlObject&, const V_Vertex&, uint color, Buffer&);

} // namespace draw_object

// Functions that draws triangles pointers arrays (old way)

namespace draw_triangles {

  int  Wired(const V_TrianglePtr&, Buffer&);
  int  Solid(const V_TrianglePtr&, Buffer&);
  int  Solid(const V_TrianglePtr&, ZBuffer&, Buffer&);

} // namespace draw_triangles

// New way to render 

namespace render {

  // Main function to rendering triangles 

  int  Context(const V_TrianglePtr&, RenderContext&);

  // Just adaptors to legacy functions
  
  int  Wired(const V_TrianglePtr& t, Buffer&b);
  int  Solid(const V_TrianglePtr&, Buffer&);
  int  Solid(const V_TrianglePtr&, ZBuffer&, Buffer&);

  // Regular functions

  int  Solid(const V_TrianglePtr&, ZBuffer&, float dist, Buffer&);
  int  Solid(const V_TrianglePtr&, ZBuffer&, float dist, const AlphaLut&, Buffer&);

} // namespace render

// Inline functions (adaptors) implementation

inline int render::Wired(const V_TrianglePtr& t, Buffer& b)
{
  return draw_triangles::Wired(t, b);
}

inline int render::Solid(const V_TrianglePtr& t, Buffer& b)
{
  return draw_triangles::Solid(t, b);
}

inline int render::Solid(const V_TrianglePtr& t, ZBuffer& z, Buffer& b)
{
  return draw_triangles::Solid(t, z, b);
}

} // namespace anshub

#endif  // GL_DRAW_H