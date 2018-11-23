// *************************************************************
// File:    gl_scr_buffer.cc
// Descr:   emulates screen buffer for rendering
// Author:  Novoselov Anton @ 2017
// *************************************************************

#include "gl_scr_buffer.h"

namespace anshub {

ScrBuffer::ScrBuffer(int w, int h, int color)
  : w_{w}
  , h_{h}
  , clear_color_{color}
  , format_{GL_BGRA}                // see note #3
  , type_{GL_UNSIGNED_INT_8_8_8_8}  //   after code
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

  glMatrixMode(GL_PROJECTION);      // todo: this is redundant since we use glWindowPos2i
  glLoadIdentity();                 //       and not glRasterPos2i
  glOrtho(0, w_, 0, h_, -1, 1);
}
  
// Sends pixel data to framebuffer

void ScrBuffer::SendDataToFB()
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

// Note #3 : we works with little-endian arch (i.e., 1000 is 0x000003e8, stored
// in memory as "e8 03 00 00" from lsb to msb). In all helpers functions we would
// work with color stored in unsigned integer as ARGB (where A is lsb, and B is 
// msb), thus in memory this would be as: b g r a). This is word-ordered format.

// Next, now we should say to opengl which format we will use:
//  - type - GL_BGRA  (we show which color components we use)
//  - format - GL_UNSIGNED_INT_8_8_8_8 (how to interpret GL_BGRA per component)

// Note, that bytes interpretation on 4 byte color is ordering from msb to lsb (by
// convient in opengl).

// Url_1: https://www.khronos.org/opengl/wiki/Pixel_Transfer#Pixel_format
// Url_2: https://en.wikipedia.org/wiki/RGBA_color_space
// Url_3: https://en.wikipedia.org/wiki/RGBA_color_space
// Url_4: http://www.laurenscorijn.com/articles/colormath-basics

// Todo: make format changing with extension ARB_internal_format_query