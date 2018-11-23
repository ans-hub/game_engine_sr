// *************************************************************
// File:    smp_window.cc
// Descr:   implementation of simple gui window
// Author:  Novoselov Anton @ 2017
// *************************************************************

#include "smp_window.h"

namespace anshub {

// Constructs the SmpWindow with default root window

SmpWindow::SmpWindow(int x, int y, int w, int h, const char* name)
  : SmpWindow(0, x, y, w, h, name)
{ }

// Constructs the SmpWindow with custom root window

SmpWindow::SmpWindow(Window, int x, int y, int w, int h, const char* name)
  : BaseWindow()
{
  scr_      = DefaultScreen(disp_);
  context_  = DefaultGC(disp_, scr_);
  self_     = io_helpers::CreateSmpWindow(disp_, root_, scr_, x, y, w, h);
  io_helpers::ChangeWindowName(disp_, self_, name);
  this->Show();
  this->Move(x,y);          // see note #1
  this->NotifyWhenClose();  // see note #2
}

void SmpWindow::SetBackgroundColor(unsigned long c)
{
  XSetWindowBackground(disp_, self_, c);
}

void SmpWindow::Clear()
{
  XClearWindow(disp_, self_);
}

} // namespace anshub

// Note 1: (hack) some window managers ignores our hints about window
//         position while creating window. Thus we move window after show
// Note 2: it is necessary call not in base class constructor since
//         there yet not created self_ instance 