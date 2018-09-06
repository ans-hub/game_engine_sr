// *************************************************************
// File:    gl_debug_draw.cc
// Descr:   draws normals, vectors, etc. in debug purposes
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#include "gl_debug_draw.h"

namespace anshub {

DebugContext::DebugContext()
  : lines_{}
  , text_{}
  , render_first_{false}
{ }

void DebugContext::AddLine(const Vector& p0, const Vector& p1, const FColor& color)
{
  lines_.push_back({p0, p1, color});
}

// Draws line by given 2 vectors. Vector are in world coordinates 

void debug_render::DrawVector(Vector begin, Vector end, const FColor& color, RenderContext& ctx)
{
  coords::World2Camera(begin, ctx.cam_->vrp_, ctx.cam_->dir_, ctx.cam_->trig_);
  coords::ClipNearZ(begin, ctx.cam_->z_near_);
  coords::Camera2Persp(begin, ctx.cam_->dov_, ctx.cam_->ar_);
  coords::Persp2Screen(begin, ctx.cam_->wov_, ctx.cam_->scr_w_, ctx.cam_->scr_h_);
    
  coords::World2Camera(end, ctx.cam_->vrp_, ctx.cam_->dir_, ctx.cam_->trig_);
  coords::ClipNearZ(end, ctx.cam_->z_near_);
  coords::Camera2Persp(end, ctx.cam_->dov_, ctx.cam_->ar_);
  coords::Persp2Screen(end, ctx.cam_->wov_, ctx.cam_->scr_w_, ctx.cam_->scr_h_);

  if ((begin.z <= ctx.cam_->z_near_ && end.z <= ctx.cam_->z_near_) ||
      (begin.z >= ctx.cam_->z_far_ && end.z >= ctx.cam_->z_far_))
    return;

  if (segment2d::Clip(
    0, 0, ctx.cam_->scr_w_-1, ctx.cam_->scr_h_-1, begin.x, begin.y, end.x, end.y)
  )
    raster::Line(begin.x, begin.y, end.x, end.y, color.GetARGB(), ctx.sbuf_);
}

} // namespace anshub