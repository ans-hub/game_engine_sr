// *************************************************************
// File:    gl_draw.h
// Descr:   represents draw functions works with linear FB
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES     // need to get functions proto from glext.h
#endif

#ifndef IO_GL_DRAW_H
#define IO_GL_DRAW_H

#include <cmath>

#include <GL/gl.h>
#include <GL/glext.h>

#include "gl_buffer.h"

namespace anshub {

namespace draw_helpers {

  void DrawPoint(int, int, int, Buffer&);
  void DrawPoint(int, int, int, uint*, int);
  void DrawLine(int, int, int, int, int, Buffer&);

} // namespace draw_helpers

} // namespace anshub

#endif  // IO_GL_DRAW_H