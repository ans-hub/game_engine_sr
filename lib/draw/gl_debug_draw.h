// *************************************************************
// File:    gl_debug_draw.h
// Descr:   draws normals, vectors, etc. in debug purposes
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef GL_DEBUG_DRAW_H
#define GL_DEBUG_DRAW_H

#include "lib/math/vector.h"
#include "lib/math/trig.h"
#include "lib/math/segment.h"

#include "lib/draw/gl_aliases.h"
#include "lib/draw/gl_object.h"
#include "lib/draw/gl_coords.h"
#include "lib/draw/fx_colors.h"
#include "lib/draw/gl_draw.h"
#include "lib/draw/gl_buffer.h"
#include "lib/draw/fx_rasterizers.h"

namespace anshub {

// Helps to hold different data for debug drawing

struct DebugContext
{
  DebugContext(Buffer& sbuf, const GlCamera& cam)
    : color_{color::Red}
    , len_multiplier_{1.0f}
    , sbuf_{sbuf}
    , cam_{cam}
  { }
  uint    color_;
  float   len_multiplier_;
  Buffer& sbuf_;
  const   GlCamera& cam_;

}; // struct DebugContext

// Debug drawing procedures

namespace debug_render {

  using cDebugCtx = const DebugContext;

  void DrawVector(cVector& vec, Vector begin, cDebugCtx&);
  void DrawLine(Vector begin, Vector end, cDebugCtx&);

} // namespace debug_draw

} // namespace anshub

#endif  // GL_DEBUG_DRAW_H