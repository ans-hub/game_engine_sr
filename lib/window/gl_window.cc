// *************************************************************
// File:    gl_window.cc
// Descr:   implementation of opengl window
// Author:  Novoselov Anton @ 2017
// URL:     https://github.com/ans-hub/iogame_lib
// *************************************************************

#include "gl_window.h"

namespace anshub {

// Constructs the GlWindow with default root window

GlWindow::GlWindow(int x, int y, int w, int h, const char* name)
  : GlWindow(Pos(x,y), Size(w,h), name)
{ }

// Constructs the GlWindow with default gl attributes

GlWindow::GlWindow(const Pos& p, const Size& sz, const char* name)
  : GlWindow(
    io_helpers::GetDefaultFBAttribs(),
    io_helpers::GetDefaultGlContextAttributes(),
    p,
    sz,
    name)
{ }

// Constructs the GlWindow with custom root window with three steps

GlWindow::GlWindow(
    const FBAttrs& fb_cfg,
    const CTAttrs& ctx_cfg,
    const Pos& p, const Size& sz, const char* name)
  : BaseWindow()
  , glxver_ { io_helpers::GetGlxVersion(disp_) }
  , fbattr_ {fb_cfg}
  , ctxattr_{ctx_cfg}
  , glxself_{0}
  , fbcfgs_ {nullptr, 0}
{ 
  using namespace io_helpers;

  // New style context creating

  if (glxver_.first > 1 || (glxver_.first == 1 && glxver_.second >= 3)) {
    fbcfgs_   = GetFBConfigs(disp_, fbattr_);
    // int best  = GetBestMSAAVisual(disp_, fbcfgs_); // see note #4 why commented
    int best  = 0;
    auto cfg  = *(fbcfgs_[best]);
    auto fbvi = ChooseGlVisual(disp_, cfg);
    swa_      = GetDefaultWinAttribs(disp_, root_, fbvi.get());
    context_  = CreateGlContext(disp_, cfg, ctxattr_);
    self_     = CreateGlWindow(disp_, root_, fbvi.get(), swa_, p.x, p.y, sz.w, sz.h);
    pbuffer_  = CreatePBuffer(disp_, sz.w, sz.h, cfg);
    if (pbuffer_)
      glXMakeContextCurrent(disp_, self_, pbuffer_, context_);
    else {
      glxself_ = glXCreateWindow(disp_, cfg, self_, NULL);
      glXMakeContextCurrent(disp_, self_, glxself_, context_);
    }
  }

  // Old style context creating

  else {
    auto fbvi = ChooseGlVisual(disp_);
    swa_      = GetDefaultWinAttribs(disp_, root_, fbvi.get());
    context_  = CreateGlContext(disp_, fbvi.get());
    self_     = CreateGlWindow(disp_, root_, fbvi.get(), swa_, p.x, p.y, sz.w, sz.h);
    glXMakeCurrent(disp_, self_, context_);
  }

  // Finally

  PrintGlInfo(std::cerr);  
  ChangeWindowName(disp_, self_, name);
  this->Show();
  this->Move(p.x, p.y);       // see note #2
  this->NotifyWhenClose();    // see note #3

  io_helpers::GetWindowDimension(disp_, self_, &width_, &height_);
}

GlWindow::~GlWindow()
{
  if (fbcfgs_.front())
    XFree(fbcfgs_.front());
  if (glxself_)
    glXDestroyWindow(disp_, glxself_);  
  if (pbuffer_)
    glXDestroyPbuffer(disp_, pbuffer_);
  glXMakeCurrent(disp_, None, NULL);
  glXDestroyContext(disp_, context_);
}

void GlWindow::Clear()
{
  glClearColor(0.5, 0.5, 0.5, 0.0); // todo: set clear color outside
  glClear(GL_COLOR_BUFFER_BIT);
}

void GlWindow::Redraw()
{ 
  glXSwapBuffers(disp_, self_);
}

void GlWindow::Exposed()
{
  XGetWindowAttributes(disp_, self_, &wa_);
  glViewport(0, 0, wa_.width, wa_.height);
}

} // namespace anshub

// Note 1: only after context_ creating we may use opengl functions
// Note 2: (hack) some window managers ignores our hints about window
//         position while creating window. Thus we move window after show
// Note 3: it is necessary call not in base class constructor since
//         there yet not created self_ instance  
// Note 4: when we draw in msaa something wrong with colors... I suppose
//         this is depends on compatibility profile of gl