// *************************************************************
// File:    gl_draw.h
// Descr:   draws objects and triangles arrays
// Author:  Novoselov Anton @ 2017
// *************************************************************

#ifndef GL_DRAW_H
#define GL_DRAW_H

#include <cmath>
#include <algorithm>

#include "fx_colors.h"
#include "gl_scr_buffer.h"
#include "gl_object.h"
#include "gl_vertex.h"
#include "gl_triangle.h"
#include "gl_z_buffer.h"
#include "gl_render_ctx.h"
#include "gl_debug_draw.h"
#include "fx_rasterizers.h"

#include "lib/math/segment.h"

namespace anshub {

//****************************************************************************
// Functions that draws objects
//****************************************************************************

namespace draw_object {

  int  Wired(const GlObject&, ScrBuffer&);
  int  Solid(const GlObject&, ScrBuffer&);
  void Normals(const GlObject&, const V_Vertex&, uint color, ScrBuffer&);

} // namespace draw_object

// Functions that draws triangles pointers arrays (old way)

namespace draw_triangles {

  int  Wired(const V_TrianglePtr&, ScrBuffer&);
  int  Solid(const V_TrianglePtr&, ScrBuffer&);
  int  Solid(const V_TrianglePtr&, ZBuffer&, ScrBuffer&);

} // namespace draw_triangles

// New way to render 

namespace render {

  // Just adaptors to legacy functions
  
  int  Wired(const V_TrianglePtr& t, ScrBuffer&b);
  int  Solid(const V_TrianglePtr&, ScrBuffer&);
  int  Solid(const V_TrianglePtr&, ZBuffer&, ScrBuffer&);

  // Main function to rendering triangles

  int  Context(const V_TrianglePtr&, RenderContext&) noexcept;
  int  Context(const V_TrianglePtr&, RenderContext&, DebugContext&) noexcept;
  int  Solid(const V_TrianglePtr&, RenderContext&) noexcept;
  int  SolidWithAlpha(const V_TrianglePtr&, RenderContext&) noexcept;

} // namespace render

// Render helpers

namespace render_helpers {

  Bitmap* ChooseMipmapLevel(Triangle*, const RenderContext&);

}

//****************************************************************************
// Inline functions (adaptors) implementation
//****************************************************************************

inline int render::Wired(const V_TrianglePtr& t, ScrBuffer& b)
{
  return draw_triangles::Wired(t, b);
}

inline int render::Solid(const V_TrianglePtr& t, ScrBuffer& b)
{
  return draw_triangles::Solid(t, b);
}

inline int render::Solid(const V_TrianglePtr& t, ZBuffer& z, ScrBuffer& b)
{
  return draw_triangles::Solid(t, z, b);
}

} // namespace anshub

#endif  // GL_DRAW_H