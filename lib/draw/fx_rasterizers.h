// *************************************************************
// File:    fx_rasterizers.h
// Descr:   rasterizes points, lines and triangles
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef FX_RASTERIZERS_H
#define FX_RASTERIZERS_H

#include <cmath>

#include "gl_buffer.h"
#include "gl_z_buffer.h"
#include "fx_colors.h"
#include "gl_vertex.h"

#include "../math/vector.h"
#include "../data/bmp_loader.h"

namespace anshub {

// Used function name prefixes:
//  - FL - flat shading (also used for const shading)
//  - GR - gourang shading

//****************************************************************************
// POINT and LINES RASTERIZERS
//****************************************************************************

namespace raster {

  void Point(int x, int y, int col, Buffer&);
  void Point(int x, int y, int col, uint* buf, int lpitch);
  void Line(int x1, int y1, int x2, int y2, int col, Buffer&);
  void Line(int x1, int y1, int x2, int y2, int col, float, float, Buffer&);
  void LineBres(int x1, int y1, int x2, int y2, int col, Buffer&);
  void LineWu(int x1, int y1, int x2, int y2, int col, Buffer&);
  void HorizontalLine(int y, int x1, int x2, int col, Buffer&);

}

//****************************************************************************
// TRIANGLE RASTERIZERS
//****************************************************************************

namespace raster_tri {

  // Triangle rasterizers without 1/z buffering

  void SolidFL(
      float px1, float px2, float px3,
      float py1, float py2, float py3,
      uint color, Buffer&
  );
  void SolidGR(
      float x1, float x2, float x3, 
      float y1, float y2, float y3, 
      uint color1, uint color2, uint color3, Buffer&
  );
  void TexturedAffine(
    cVector& p1, cVector& p2, cVector& p3,
    cVector& t1, cVector& t2, cVector& t3,
    Bitmap*, Buffer&
  );
  void TexturedAffineFL(                        // optimized
    cVector& p1, cVector& p2, cVector& p3,
    cVector& t1, cVector& t2, cVector& t3,
    uint color, Bitmap*, Buffer&
  );
  void TexturedAffineGR(                        // optimized
    cVector& p1, cVector& p2, cVector& p3,
    cVector& t1, cVector& t2, cVector& t3,
    uint c1, uint c2, uint c3, Bitmap*, Buffer&
  );

  // Rasterizes triangle with 1/z-buffering

  void SolidFL(
    cVertex& v1, cVertex& v2, cVertex& v3,
    cFColor& color, ZBuffer&, Buffer&
  );
  void SolidGR(
    cVertex& v1, cVertex& v2, cVertex& v3,
    ZBuffer&, Buffer&
  );
  int TexturedPerspective(                      // optimized
    cVertex& v1, cVertex& v2, cVertex& v3,
    Bitmap*, ZBuffer&, Buffer&
  );
  int TexturedPerspectiveFL(                    // optimized
    cVertex& v1, cVertex& v2, cVertex& v3,
    cFColor& color, Bitmap*, ZBuffer&, Buffer&
  );
  int TexturedPerspectiveGR(                    // optimized
    cVertex& v1, cVertex& v2, cVertex& v3,
    Bitmap*, ZBuffer&, Buffer&
  );
  int TexturedAffineGR(                         // optimized
    cVertex& v1, cVertex& v2, cVertex& v3,
    Bitmap*, ZBuffer&, Buffer&    
  );

} // namespace raster_tri

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

inline void raster::HorizontalLine(int y, int x1, int x2, int color, Buffer& buf)
{
  auto* ptr = buf.GetPointer();
  std::fill_n(ptr + x1 + y * buf.Width(), x2-x1, color);
}

} // namespace anshub

#endif  // FX_RASTERIZERS_H

// Note : we wouldn`t recieve Face or Triangle to the rasterization functions
// since we want general way to rasterize triangles (from Object or from V_Triangle) 

// Todo : as class Rasterize.
// Usage:
//  raster.Triangle(tri, tri.shading_, );
//  raster.Point(p1, c1);
//  raster.Line(p1, p2, c1, c2);
