// *************************************************************
// File:    gl_window.h
// Descr:   implementation of opengl window
// Author:  Novoselov Anton @ 2017
// URL:     https://github.com/ans-hub/iogame_lib
// *************************************************************

#include <iostream>
#include <memory>

#include "enums.h"
#include "aliases.h"
#include "helpers.h"
#include "base_window.h"

#ifndef IO_GL_WINDOW_H
#define IO_GL_WINDOW_H

namespace anshub {

class GlWindow : public BaseWindow
{
public:

  GlWindow(int, int, int, int, const char*);
  GlWindow(const Pos&, const Size&, const char*);
  GlWindow(const FBAttrs&, const CTAttrs&, const Pos&, const Size&, const char*);

  GlWindow(const GlWindow&) =delete;
  GlWindow(GlWindow&&) =delete;

  ~GlWindow() override;

  virtual void Clear() override;
  virtual void Redraw() override;
  virtual void Exposed() override;

  void VerticalSync(bool);

private:
  Pair        glxver_;              // glx version used
  FBAttrs     fbattr_;              // gl framebuffer attributes
  CTAttrs     ctxattr_;             // gl context attributes
  GLXContext  context_;             // gl context
  GLXPbuffer  pbuffer_;             // gl pbuffer
  GLXWindow   glxself_;             //    
  FBConfigs   fbcfgs_;              // gl framebuffer configs (see note #1)

  // Helpers placed here to prevent creating it every time in loop

  WAttribs    wa_;                  // XWindowAttriubutes
  SWAttribs   swa_;                 // XSetWindowAttributes

}; // class GlWindow

}  // namespace anshub

#endif  // IO_GL_WINDOW_H

// Note #1 : fbconfigs implemented as std::vector of pointers, but not unique
// ptr since we need size of fbconfig array. Thus deletion of this struct is in
// destructor
// Note #2 : visual implemented as unique ptr with custom deleter