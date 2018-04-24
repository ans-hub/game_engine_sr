// *************************************************************
// File:    gl_debug_draw.cc
// Descr:   draws normals, vectors, etc. in debug purposes
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#include "gl_debug_draw.h"

namespace anshub {

// Draws vector from pos with given color. Vec and pos should be in the
// world coordinates

void debug_render::DrawVector(cVector& vec, Vector begin, cDebugCtx& ctx)
{
  // Define line`s begin and end

  Vector end {begin + (vec * ctx.len_multiplier_)};
  if (math::Fzero(begin.z) || math::Fzero(end.z))
  {
    begin.z = math::kEpsilon * 2.0f;  // prevent zero dividing
    end.z = math::kEpsilon * 2.0f;
  }
  debug_render::DrawLine(begin, end, ctx);
}

// Draws line by given 2 vectors. Vector are in world coordinates 

void debug_render::DrawLine(Vector begin, Vector end, cDebugCtx& ctx)
{
  if (begin.IsZero() || end.IsZero())
    return;

  coords::World2Camera(begin, ctx.cam_.vrp_, ctx.cam_.dir_, ctx.cam_.trig_);
  coords::Camera2Persp(begin, ctx.cam_.dov_, ctx.cam_.ar_);
  coords::Persp2Screen(begin, ctx.cam_.wov_, ctx.cam_.scr_w_, ctx.cam_.scr_h_);
    
  coords::World2Camera(end, ctx.cam_.vrp_, ctx.cam_.dir_, ctx.cam_.trig_);
  coords::Camera2Persp(end, ctx.cam_.dov_, ctx.cam_.ar_);
  coords::Persp2Screen(end, ctx.cam_.wov_, ctx.cam_.scr_w_, ctx.cam_.scr_h_);
  
  if (begin.z <= ctx.cam_.z_near_ || end.z <= ctx.cam_.z_near_)
    return;
    
  if (segment2d::Clip(
    0, 0, ctx.cam_.scr_w_, ctx.cam_.scr_h_, begin.x, begin.y, end.x, end.y)
  )
    raster::Line(begin.x, begin.y, end.x, end.y, ctx.color_, ctx.sbuf_);
}

} // namespace anshub