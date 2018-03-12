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
#include "gl_raster_tri.h"

#include "../math/segment.h"

namespace anshub {

// Functions that draws objects

namespace draw_object {

  int  Wired(const GlObject&, Buffer&);
  int  Solid(const GlObject&, Buffer&);
  void Normals(const GlObject&, const V_Vertex&, uint color, Buffer&);

} // namespace draw_object

// Functions that draws triangles pointers arrays

namespace draw_triangles {

  int  Wired(const V_TrianglePtr&, Buffer&);
  int  Solid(const V_TrianglePtr&, Buffer&);
  int  Solid(const V_TrianglePtr&, ZBuffer&, Buffer&);

} // namespace draw_triangles

} // namespace anshub

#endif  // GL_DRAW_H