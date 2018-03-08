// *************************************************************
// File:    gl_raster_point.h
// Descr:   rasterizes point
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef GL_RASTER_POINT_H
#define GL_RASTER_POINT_H

#include "gl_buffer.h"

namespace anshub {

namespace raster {

  void Point(int x, int y, int color, Buffer&);
  void Point(int x, int y, int color, uint* buf, int lpitch);

} // namespace raster

//****************************************************************************
// Implementations of inline functions
//****************************************************************************

// Draws point using Buffer object

inline void raster::Point(int x, int y, int color, Buffer& buf)
{
  buf[x + y * buf.Width()] = color;
}

// Draws point using buffer pointer

inline void raster::Point(int x, int y, int color, uint* buf, int lpitch)
{
  buf[x + y * lpitch] = color;
}

} // namespace anshub

#endif  // GL_RASTER_POINT_H