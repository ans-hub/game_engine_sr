// *************************************************************
// File:    gl_rasterize_tri_z.h
// Descr:   rasterizes triangles with 1/z buffer
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef GL_RASTERIZE_TRI_Z_H
#define GL_RASTERIZE_TRI_Z_H

#include <cmath>

#include "gl_buffer.h"
#include "gl_z_buffer.h"
#include "fx_colors.h"
#include "gl_vertex.h"
#include "gl_rasterize_line.h"
#include "gl_rasterize_point.h"
#include "../math/vector.h"
#include "../data/bmp_loader.h"

namespace anshub {

namespace draw {

  // Rasterizes triangles (with 1/z-buffer)

  // void SolidTriangle(ZBuffer&, Buffer&);
  // void GourangTriangle(
  //   float, float, float, float, float, float, uint, uint, uint, Buffer&);
  void TexturedTriangle(
    cVertex&, cVertex&, cVertex&, Bitmap*, ZBuffer&, Buffer&);
  // void TexturedTriangleLight(
  //   Bitmap*, cVector&, cVector&, cVector&, cVector&, cVector&, cVector&,
  //   uint color, Buffer&);

} // namespace draw

} // namespace anshub

#endif  // GL_RASTERIZE_TRI_Z_H