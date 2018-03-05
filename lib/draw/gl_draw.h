// *************************************************************
// File:    gl_draw.h
// Descr:   represents draw functions works with linear FB
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES     // need to get functions proto from glext.h
#endif

#ifndef GL_DRAW_H
#define GL_DRAW_H

#include <cmath>
#include <algorithm>

#include <GL/gl.h>
#include <GL/glext.h>

#include "../math/vector.h"
#include "../data/bmp_loader.h"
#include "gl_buffer.h"
#include "fx_colors.h"
#include "gl_object.h"
#include "gl_triangle.h"

namespace anshub {

namespace draw {

  // Primitives

  void Point(int, int, int, Buffer&);
  void Point(int, int, int, uint*, int);  // todo: explain
  void HorizontalLine(int y, int x1, int x2, int color, Buffer&);
  void LineBres(int, int, int, int, int, Buffer&);
  void Line(int, int, int, int, int color, Buffer&);
  void Line(int, int, int, int, int, float br_1, float br_2, Buffer&);
  void LineWu(int, int, int, int, int, Buffer&);
  void SolidTriangle(
    float, float, float, float, float, float, uint color, Buffer&);
  void GourangTriangle(
    float, float, float, float, float, float, uint, uint, uint, Buffer&);
  void TexturedTriangle(
    const Bitmap&, cVector&, cVector&, cVector&, cVector&, cVector&, cVector&,
    Buffer&);
  void TexturedTriangleLight(
    const Bitmap&, cVector&, cVector&, cVector&, cVector&, cVector&, cVector&,
    uint color, Buffer&);

  // Complex

  void WiredObject(const GlObject&, Buffer&);
  int  SolidObject(const GlObject&, Buffer&);
  void WiredTriangles(const V_Triangle&, Buffer&);
  int  SolidTriangles(const V_Triangle&, Buffer&);

  // Debug
  
  void ObjectNormals(const GlObject&, const V_Vertex&, uint color, Buffer&);

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

#endif  // GL_DRAW_H