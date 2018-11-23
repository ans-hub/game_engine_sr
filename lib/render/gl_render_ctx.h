// *************************************************************
// File:    gl_render_ctx.h
// Descr:   struct to hold context of rendering
// Author:  Novoselov Anton @ 2017
// *************************************************************

#ifndef GL_RENDER_CTX_H
#define GL_RENDER_CTX_H

#include "gl_scr_buffer.h"
#include "gl_z_buffer.h"
#include "cameras/gl_camera.h"

namespace anshub {

//***************************************************************************
// Represents struct to hold different information about rendering
// and used in render functions
//***************************************************************************

struct RenderContext
{
  RenderContext(int w, int h, int color);

  bool    is_wired_;
  bool    is_alpha_;
  bool    is_zbuf_;
  bool    is_bifiltering_;
  bool    is_mipmapping_;
  float   clarity_;
  float   mipmap_dist_;
  int     pixels_drawn_;
  int     triangles_drawn_;

  GlCamera* cam_;
  ScrBuffer sbuf_;
  ZBuffer   zbuf_;

}; // struct RenderContext

//***************************************************************************
// Inline implementation
//***************************************************************************

inline RenderContext::RenderContext(int w, int h, int color)
  : is_wired_{false}
  , is_alpha_{false}
  , is_zbuf_{true}
  , is_bifiltering_{false}
  , is_mipmapping_{false}
  , clarity_{1.0f}
  , mipmap_dist_{1.0f}
  , pixels_drawn_{}
  , triangles_drawn_{}
  , cam_{nullptr}
  , sbuf_{w, h, color}
  , zbuf_{w, h}
{ }

}  // namespace anshub

#endif  // GL_RENDER_CTX_H