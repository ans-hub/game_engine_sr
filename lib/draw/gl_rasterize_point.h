// *************************************************************
// File:    gl_rasterize_point.h
// Descr:   rasterizes point
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef GL_RASTERIZE_POINT_H
#define GL_RASTERIZE_POINT_H

#include "gl_buffer.h"

namespace anshub {

namespace draw {

  // Points

  void Point(int, int, int, Buffer&);
  void Point(int, int, int, uint*, int);  // todo: explain
  void HorizontalLine(int y, int x1, int x2, int color, Buffer&);

} // namespace draw

//****************************************************************************
// Implementations of inline functions
//****************************************************************************

// Draws point using Buffer object

inline void draw::Point(int x, int y, int color, Buffer& buf)
{
  buf[x + y * buf.Width()] = color;
}

// Draws point using buffer pointer

inline void draw::Point(int x, int y, int color, uint* buf, int lpitch)
{
  buf[x + y * lpitch] = color;
}

inline void draw::HorizontalLine(int y, int x1, int x2, int color, Buffer& buf)
{
  auto* ptr = buf.GetPointer();
  std::fill_n(ptr + x1 + y * buf.Width(), x2-x1, color);
}

} // namespace anshub

#endif  // GL_RASTERIZE_POINT_H