// *************************************************************
// File:    gl_raster_tri.h
// Descr:   rasterizes triangles
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef GL_RASTER_TRI_H
#define GL_RASTER_TRI_H

#include <cmath>

#include "gl_buffer.h"
#include "gl_z_buffer.h"
#include "fx_colors.h"
#include "gl_vertex.h"
#include "gl_raster_line.h"
#include "gl_raster_point.h"
#include "../math/vector.h"
#include "../data/bmp_loader.h"

namespace anshub {

namespace raster {

  // Rasterizes triangle (without z-buffering)

  void SolidTriangle(
      float px1, float px2, float px3,
      float py1, float py2, float py3,
      uint color, Buffer&
  );
  void GourangTriangle(
      float x1, float x2, float x3, 
      float y1, float y2, float y3, 
      uint color1, uint color2, uint color3, Buffer&
  );
  void TexturedTriangle(
    cVector& p1, cVector& p2, cVector& p3,
    cVector& t1, cVector& t2, cVector& t3,
    Bitmap*, Buffer&
  );
  void TexturedTriangleFL(
    cVector& p1, cVector& p2, cVector& p3,
    cVector& t1, cVector& t2, cVector& t3,
    uint color, Bitmap*, Buffer&
  );

  // Rasterizes triangle (with 1/z-buffering)

  void SolidTriangle(
    cVertex& v1, cVertex& v2, cVertex& v3,
    uint color, ZBuffer&, Buffer&
  );
  void GourangTriangle(
    cVertex& v1, cVertex& v2, cVertex& v3,
    ZBuffer&, Buffer&
  );
  void TexturedTriangle(
    cVertex& v1, cVertex& v2, cVertex& v3,
    Bitmap*, ZBuffer&, Buffer&
  );
  void TexturedTriangleFL(                    // highly optimized
    cVertex& v1, cVertex& v2, cVertex& v3,
    uint color, Bitmap*, ZBuffer&, Buffer&
  );

} // namespace raster

} // namespace anshub

#endif  // GL_RASTER_TRI_H

// Note : we wouldn`t recieve Face or Triangle to the rasterization functions
// since we want general way to rasterize triangles (from Object or from V_Triangle) 