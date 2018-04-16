// *************************************************************
// File:    gl_text.h
// Descr:   text output for opengl (req <= gl3.0 or comp.mode)
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES     // need to get functions proto from glext.h
#endif

#ifndef IO_GL_TEXT_H
#define IO_GL_TEXT_H

#include <X11/X.h>
#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>
#include <GL/glext.h>

#include "lib/window/gl_window.h"
#include "lib/window/aliases.h"
#include "lib/window/exceptions.h"

namespace anshub {

constexpr const char* default_font_name_ = "-*-*-*-*-*-*-14-*-*-*-*-*-*-*";

class GlText
{
public:
  explicit GlText(GlWindow&);
  GlText(GlWindow&, const char*);
  ~GlText();

  void PrintString(int, int, const char*);

private:
  Display*      disp_;
  Window        self_;
  XFontStruct*  font_;  
  GLuint        base_;
  WAttribs      gwa_;

  void SetStringPos(int, int, int, int);

}; // class GlText

}  // namespace anshub

#endif  // IO_GL_TEXT_H