// *************************************************************
// File:    gl_draw_objects.h
// Descr:   draws objects
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef GL_DRAW_OBJECTS_H
#define GL_DRAW_OBJECTS_H

#include <cmath>
#include <algorithm>

#include "../math/segment.h"
#include "gl_buffer.h"
#include "fx_colors.h"
#include "gl_triangle.h"
#include "gl_rasterize_point.h"
#include "gl_rasterize_line.h"
#include "gl_rasterize_tri.h"

namespace anshub {

namespace draw {

  // Objects

  void WiredObject(const GlObject&, Buffer&);
  int  SolidObject(const GlObject&, Buffer&);
  void ObjectNormals(const GlObject&, const V_Vertex&, uint color, Buffer&);

} // namespace draw

} // namespace anshub

#endif  // GL_DRAW_OBJECTS_H