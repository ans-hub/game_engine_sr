// *************************************************************
// File:    gl_raster_line.h
// Descr:   rasterizes lines
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef GL_RASTER_LINE_H
#define GL_RASTER_LINE_H

#include <cmath>

#include "fx_colors.h"
#include "gl_buffer.h"
#include "gl_raster_point.h"
#include "../math/vector.h"

namespace anshub {

namespace raster {

  void Line(int x1, int y1, int x2, int y2, int color, Buffer&);
  void Line(
    int x1, int y1, int x2, int y2,
    int color, float bright1, float bright2, Buffer&);
  void LineBres(int x1, int y1, int x2, int y2, int color, Buffer&);
  void LineWu(int x1, int y1, int x2, int y2, int color, Buffer&);
  void HorizontalLine(int y, int x1, int x2, int color, Buffer&);

} // namespace raster

inline void raster::HorizontalLine(int y, int x1, int x2, int color, Buffer& buf)
{
  auto* ptr = buf.GetPointer();
  std::fill_n(ptr + x1 + y * buf.Width(), x2-x1, color);
}

} // namespace anshub

#endif  // GL_RASTER_LINE_H