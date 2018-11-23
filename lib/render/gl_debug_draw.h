// *************************************************************
// File:    gl_debug_draw.h
// Descr:   draws normals, vectors, etc. in debug purposes
// Author:  Novoselov Anton @ 2017
// *************************************************************

#ifndef GL_DEBUG_DRAW_H
#define GL_DEBUG_DRAW_H

#include "lib/math/vector.h"
#include "lib/math/trig.h"
#include "lib/math/segment.h"

#include "lib/render/gl_aliases.h"
#include "lib/render/gl_object.h"
#include "lib/render/gl_coords.h"
#include "lib/render/fx_colors.h"
#include "lib/render/gl_render_ctx.h"
#include "lib/render/gl_scr_buffer.h"
#include "lib/render/fx_rasterizers.h"

namespace anshub {

//***************************************************************************
// Helps to hold different data for debug drawing
//***************************************************************************

struct DebugContext
{
  struct Line
  {
    Vector begin_;
    Vector end_;
    FColor color_;

  }; // struct Line

  DebugContext();
  void AddLine(const Vector& p0, const Vector& p1, const FColor& color);

  std::vector<Line> lines_;
  std::vector<std::string> text_;
  bool render_first_;

}; // struct DebugContext

//***************************************************************************
// Debug drawing procedures
//***************************************************************************

namespace debug_render {

  void DrawVector(Vector begin, Vector end, const FColor&, RenderContext&);

} // namespace debug_draw

} // namespace anshub

#endif  // GL_DEBUG_DRAW_H