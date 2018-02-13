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

#include <GL/gl.h>
#include <GL/glext.h>

#include "../math/vector.h"
#include "gl_buffer.h"
#include "fx_colors.h"
#include "gl_object.h"
#include "gl_triangle.h"

namespace anshub {

namespace draw {

  // Algorithms

  void DrawPoint(int, int, int, Buffer&);
  void DrawPoint(int, int, int, uint*, int);
  void DrawLineBres(int, int, int, int, int, Buffer&);
  void DrawLine(int, int, int, int, int color, Buffer&);
  void DrawLine(int, int, int, int, int, float br_1, float br_2, Buffer&);
  void DrawLineWu(int, int, int, int, int, Buffer&);
  
  // Helpers

  void Object(const GlObject&, int w, int h, Buffer&);
  void Objects(const std::vector<GlObject>&, int w, int h, Buffer&);
  void TrianglesArray(const Triangles&, int w, int h, Buffer&);

} // namespace draw

//****************************************************************************
// Implementations of inline functions
//****************************************************************************

// Draws point using Buffer object

inline void draw::DrawPoint(int x, int y, int color, Buffer& buf)
{
  buf[x + y * buf.Width()] = color;
}

// Draws point using buffer pointer

inline void draw::DrawPoint(int x, int y, int color, uint* buf, int lpitch)
{
  buf[x + y * lpitch] = color;
}

} // namespace anshub

#endif  // GL_DRAW_H