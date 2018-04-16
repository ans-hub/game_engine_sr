// *************************************************************
// File:    fx_rasterizers.h
// Descr:   rasterizes points, lines and triangles
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef FX_RASTERIZERS_H
#define FX_RASTERIZERS_H

#include <cmath>

#include "lib/draw/gl_buffer.h"
#include "lib/draw/gl_z_buffer.h"
#include "lib/draw/fx_colors.h"
#include "lib/draw/gl_vertex.h"

#include "lib/math/vector.h"
#include "lib/data/bmp_loader.h"

namespace anshub {

// Used function name prefixes:
//  - FL - flat shading (also used for const shading)
//  - GR - gouraud shading
//  - BF - bilinear texture filtering

//****************************************************************************
// POINT and LINES RASTERIZERS
//****************************************************************************

namespace raster {

  void Point(int x, int y, int col, Buffer&) noexcept;
  void Point(int x, int y, int col, uint* buf, int lpitch) noexcept;
  void Line(int x1, int y1, int x2, int y2, int col, Buffer&) noexcept;
  void Line(int x1, int y1, int x2, int y2, int col, float, float, Buffer&) noexcept;
  void LineBres(int x1, int y1, int x2, int y2, int col, Buffer&) noexcept;
  void LineWu(int x1, int y1, int x2, int y2, int col, Buffer&) noexcept;
  void HorizontalLine(int y, int x1, int x2, int col, Buffer&) noexcept;

}

//****************************************************************************
// TRIANGLE RASTERIZERS
//****************************************************************************

namespace raster_tri {

  // Triangle rasterizers without 1/z buffering

  void SolidFL(                                 // v1
      float px1, float px2, float px3,
      float py1, float py2, float py3,
      uint color, Buffer&
  ) noexcept;
  void SolidGR(                                 // v1
      float x1, float x2, float x3, 
      float y1, float y2, float y3, 
      uint c1, uint c2, uint c3, Buffer&
  ) noexcept;
  void TexturedAffine(                          // v1
    cVector& p1, cVector& p2, cVector& p3,
    cVector& t1, cVector& t2, cVector& t3,
    Bitmap*, Buffer&
  ) noexcept;
  void TexturedAffineFL(                        // v1
    cVector& p1, cVector& p2, cVector& p3,
    cVector& t1, cVector& t2, cVector& t3,
    uint color, Bitmap*, Buffer&
  ) noexcept;
  void TexturedAffineGR(                        // v1
    cVector& p1, cVector& p2, cVector& p3,
    cVector& t1, cVector& t2, cVector& t3,
    uint c1, uint c2, uint c3, Bitmap*,
    Buffer&
  ) noexcept;

  // Rasterizes triangle with 1/z-buffering

  int SolidFL(                                  // v2, optimized +
    Vertex v1, Vertex v2, Vertex v3,
    cFColor& color, ZBuffer&, Buffer&
  ) noexcept;
  int SolidGR(                                  // v2, optimized +
    Vertex v1, Vertex v2, Vertex v3,
    ZBuffer&, Buffer&
  ) noexcept;
  int TexturedPerspective(                      // v2, optimized +
    Vertex v1, Vertex v2, Vertex v3,
    Bitmap*, ZBuffer&, Buffer&
  ) noexcept;
  int TexturedPerspectiveFL(                    // v2, optimized +
    Vertex v1, Vertex v2, Vertex v3,
    cFColor& color, Bitmap*, ZBuffer&, Buffer&
  ) noexcept;
  int TexturedPerspectiveFLBF(                  // v2, optimized +
    Vertex v1, Vertex v2, Vertex v3,
    cFColor& color, Bitmap*, ZBuffer&, Buffer&
  ) noexcept;
  int TexturedPerspectiveGR(                    // v2, optimized +
    Vertex v1, Vertex v2, Vertex v3,
    Bitmap*, ZBuffer&, Buffer&
  ) noexcept;
  int TexturedAffineGR(                         // v2, optimized +
    Vertex v1, Vertex v2, Vertex v3,
    Bitmap*, ZBuffer&, Buffer&    
  ) noexcept;
  int TexturedAffineGRBF(                       // v2, optimized +
    Vertex v1, Vertex v2, Vertex v3,
    Bitmap*, ZBuffer&, Buffer&    
  ) noexcept;

} // namespace raster_tri


//****************************************************************************
// RASTERIZERS HELPERS
//****************************************************************************

namespace raster_helpers {

  void SortVertices(Vertex&, Vertex&, Vertex&) noexcept;
  void UnnormalizeTexture(Vertex&, Vertex&, Vertex&, int w, int h) noexcept;

} // namespace raster_helpers

//****************************************************************************
// Implementations of inline functions
//****************************************************************************

// Draws point using Buffer object

inline void raster::Point(
  int x, int y, int color, Buffer& buf) noexcept
{
  buf[x + y * buf.Width()] = color;
}

// Draws point using buffer pointer

inline void raster::Point(
  int x, int y, int color, uint* buf, int lpitch) noexcept
{
  buf[x + y * lpitch] = color;
}

inline void raster::HorizontalLine(
  int y, int x1, int x2, int color, Buffer& buf) noexcept
{
  auto* ptr = buf.GetPointer();
  std::fill_n(ptr + x1 + y * buf.Width(), x2-x1, color);
}

// Sort vertices in order - v1 is the most top and v3 is the bottom

inline void raster_helpers::SortVertices(
  Vertex& v1, Vertex& v2, Vertex& v3) noexcept
{
  // Make v1 as top, v2 as middle, v3 as bottom

  if (v2.pos_.y < v3.pos_.y)
    std::swap(v2, v3);
  if ((v1.pos_.y < v2.pos_.y) && (v1.pos_.y > v3.pos_.y))
    std::swap(v1, v2);
  else if ((v1.pos_.y < v2.pos_.y) && 
           (v1.pos_.y < v3.pos_.y || math::Feq(v1.pos_.y, v3.pos_.y))) {
    std::swap(v1, v2);
    std::swap(v3, v2);  
  }

  // If polygon is flat bottom, sort left to right

  if (math::Feq(v2.pos_.y, v3.pos_.y) && v2.pos_.x > v3.pos_.x)
    std::swap(v2, v3);

  // If polygon is flat top, sort left to right

  if (math::Feq(v1.pos_.y, v2.pos_.y) && v1.pos_.x > v2.pos_.x)
    std::swap(v1, v2);
}

// Get real texture coordinates from normalized coordinates

inline void raster_helpers::UnnormalizeTexture(
  Vertex& v1, Vertex& v2, Vertex& v3, int w, int h) noexcept
{
  v1.texture_.x *= w-1;
  v2.texture_.x *= w-1;
  v3.texture_.x *= w-1;
  v1.texture_.y *= h-1;
  v2.texture_.y *= h-1;
  v3.texture_.y *= h-1;
}

} // namespace anshub

#endif  // FX_RASTERIZERS_H