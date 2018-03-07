// *************************************************************
// File:    gl_draw_triangles.h
// Descr:   draws triangles arrays
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef GL_DRAW_TRIANGLES_H
#define GL_DRAW_TRIANGLES_H

#include <cmath>
#include <algorithm>

#include "../math/segment.h"
#include "gl_buffer.h"
#include "fx_colors.h"
#include "gl_triangle.h"
#include "gl_z_buffer.h"
#include "gl_rasterize_point.h"
#include "gl_rasterize_line.h"
#include "gl_rasterize_tri.h"
#include "gl_rasterize_tri_z.h"

namespace anshub {

namespace draw {

  // Draws triangles arrays

  void WiredTriangles(const V_Triangle&, Buffer&);
  int  SolidTriangles(const V_Triangle&, Buffer&);
  int  SolidTrianglesZ(const V_Triangle&, ZBuffer&, Buffer&);

} // namespace draw

} // namespace anshub

#endif  // GL_DRAW_TRIANGLES_H