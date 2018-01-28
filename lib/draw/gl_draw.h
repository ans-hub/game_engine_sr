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

#include "gl_buffer.h"
#include "fx_colors.h"

namespace anshub {

namespace draw {

  void DrawPoint(int, int, int, Buffer&);
  void DrawPoint(int, int, int, uint*, int);
  void DrawLineBres(int, int, int, int, int, Buffer&);
  void DrawLine(int, int, int, int, int color, Buffer&);
  void DrawLine(int, int, int, int, int, double br_1, double br_2, Buffer&);
  void DrawLineWu(int, int, int, int, int, Buffer&);

} // namespace draw_helpers

} // namespace anshub

#endif  // GL_DRAW_H