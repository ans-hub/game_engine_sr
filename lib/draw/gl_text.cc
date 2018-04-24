// *************************************************************
// File:    gl_text.cc
// Descr:   text output for opengl (req <= gl3.0 or comp.mode)
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#include "gl_text.h"

namespace anshub {

GlText::GlText(GlWindow& win) : GlText(win, gl_text::default_font_) { }

GlText::GlText(GlWindow& win, const char* font_name)
  : disp_{win.Disp()}
  , self_{win.Self()}
  , font_{nullptr}
  , base_{0}
{
  auto gl_ver = io_helpers::GetGlContextVersion();
  auto gl_prf = io_helpers::GetGlContextProfile();
  
  if (gl_ver.first >= 3 && gl_ver.second > 0) {
    if (gl_prf.first != 2)
      throw IOException("GlText: gl context is not in compatible mode", errno);      
  }

  if (!disp_)
    throw IOException("GlText: Can't open display", -1);

  font_ = XLoadQueryFont(disp_, font_name);
  if (!font_)
    throw IOException("GlText: Can't open font", errno);
  
  base_ = glGenLists(96);
  if (!base_)
    throw IOException("GlText: Can't generate lists", errno);

  glXUseXFont(font_->fid, 32, 96, base_);
}

GlText::~GlText()
{
  if (font_)
    XFreeFont(disp_, font_);
  if (base_)
    glDeleteLists(base_, 96);   // see note #1
}

void GlText::PrintString(int x, int y, const char* str)
{
  XGetWindowAttributes(disp_, self_, &gwa_);
  SetStringPos(gwa_.width, gwa_.height, x, y);

  if (str && strlen(str)) {
    glPushAttrib(GL_LIST_BIT);
    glListBase(base_ - 32);
    glCallLists((int)strlen(str), GL_UNSIGNED_BYTE, (GLubyte*)str);
    glPopAttrib();
  }
}

void GlText::SetStringPos(int w, int h, int x, int y)
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, (float)w, 0, (float)h, -1., 1.);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glColor3d(255, 255, 255);
  glRasterPos2i(x, y);
}

} // namespace anshub

// Note #1 : seems to be a bug in r600_dri.so, leaks