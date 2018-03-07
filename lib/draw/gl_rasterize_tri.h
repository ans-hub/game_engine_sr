// *************************************************************
// File:    gl_rasterize_tri.h
// Descr:   rasterizes triangles
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef GL_RASTERIZE_TRI_H
#define GL_RASTERIZE_TRI_H

#include <cmath>

#include "gl_buffer.h"
#include "fx_colors.h"
#include "gl_rasterize_line.h"
#include "gl_rasterize_point.h"
#include "../math/vector.h"
#include "../data/bmp_loader.h"

namespace anshub {

namespace draw {

  // Rasterizes triangles (without z-buffering)

  void SolidTriangle(
    float, float, float, float, float, float, uint color, Buffer&);
  void GourangTriangle(
    float, float, float, float, float, float, uint, uint, uint, Buffer&);
  void TexturedTriangle(
    Bitmap*, cVector&, cVector&, cVector&, cVector&, cVector&, cVector&,
    Buffer&);
  void TexturedTriangleLight(
    Bitmap*, cVector&, cVector&, cVector&, cVector&, cVector&, cVector&,
    uint color, Buffer&);

} // namespace draw

} // namespace anshub

#endif  // GL_DRAW_H