// *************************************************************
// File:    gl_render_ctx.h
// Descr:   struct to hold context of rendering
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef GL_RENDER_CTX_H
#define GL_RENDER_CTX_H

#include "gl_buffer.h"
#include "gl_z_buffer.h"

namespace anshub {

// Represents struct to hold different information about rendering
// and used in render functions

struct RenderContext
{
  RenderContext(int w, int h, int color)
    : is_wired_{false}
    , is_alpha_{false}
    , is_zbuf_{true}
    , is_bifiltering_{false}
    , clarity_{}
    , pixels_drawn_{}
    , sbuf_{w, h, color}
    , zbuf_{w, h}
  { }

  // Context members

  bool is_wired_;
  bool is_alpha_;
  bool is_zbuf_;
  bool is_bifiltering_;
  int  clarity_;
  int  pixels_drawn_;

  // Context entities

  Buffer    sbuf_;
  ZBuffer   zbuf_;

}; // struct RenderContext

}  // namespace anshub

#endif  // GL_RENDER_CTX_H