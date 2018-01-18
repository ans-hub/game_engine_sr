// *************************************************************
// File:    gl_buffer.cc
// Descr:   emulates buffer for manual rendering
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#include "gl_buffer.h"

namespace anshub {

Buffer::Buffer(int w, int h, int color)
  : w_{w}
  , h_{h}
  , clear_color_{color}
  , format_{GL_BGRA}                // see notes in math/fx_colors.cc
  , type_{GL_UNSIGNED_INT_8_8_8_8}  // after all code
  , ptr_(w_ * h_, clear_color_)
{
  // Prepare OpenGl states before using SendDataToFB()

  glShadeModel(GL_FLAT);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);
  glDrawBuffer(GL_BACK);

  glMatrixMode(GL_PROJECTION);      // this is redundant since we use glWindowPos2i
  glLoadIdentity();                 // and not glRasterPos2i
  glOrtho(0, w_, 0, h_, -1, 1);
}

void Buffer::Clear()
{
  memset(ptr_.data(), 0, w_*h_*sizeof(*ptr_.data())); 
  
  // ... forced to use memset instead std::fill after profiling
}
  
// Sends pixel data to framebuffer

void Buffer::SendDataToFB()
{
  glWindowPos2i(0, 0);  // to prevent text clipping
  glDrawPixels(w_, h_, format_, type_, (GLvoid*)ptr_.data());
}

} // namespace anshub

// Note #1 : average speed of recieving data to framebuffer:
// - 1920*1080 ~ 8.9MB * 30 fps = 237MB/s
// - this value is more less than any agp/pci-e bus in low-end pcs

// Note #2 : alternatives to glDrawPixels:
// - https://goo.gl/oe9sJc
// - https://goo.gl/3HAenc
// - glDrawArray