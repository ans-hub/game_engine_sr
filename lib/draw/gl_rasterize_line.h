// *************************************************************
// File:    gl_rasterize_line.h
// Descr:   rasterizes lines
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************


#ifndef GL_RASTERIZE_LINE_H
#define GL_RASTERIZE_LINE_H

#include <cmath>

#include "../math/vector.h"
#include "gl_buffer.h"
#include "fx_colors.h"
#include "gl_rasterize_point.h"

namespace anshub {

namespace draw {

  // Lines rasterization functions

  void HorizontalLine(int y, int x1, int x2, int color, Buffer&);
  void LineBres(int, int, int, int, int, Buffer&);
  void Line(int, int, int, int, int color, Buffer&);
  void Line(int, int, int, int, int, float br_1, float br_2, Buffer&);
  void LineWu(int, int, int, int, int, Buffer&);

} // namespace draw

} // namespace anshub

#endif  // GL_RASTERIZE_LINE_H