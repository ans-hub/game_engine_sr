// *************************************************************
// File:    smp_window.h
// Descr:   implementation of simple gui window
// Author:  Novoselov Anton @ 2017
// *************************************************************

#include "helpers.h"
#include "base_window.h"

#ifndef IO_SMP_WINDOW_H
#define IO_SMP_WINDOW_H

namespace anshub {

class SmpWindow : public BaseWindow
{
public:
  explicit SmpWindow(int, int, int, int, const char*);  // default root win
  SmpWindow(Window, int, int, int, int, const char*);   // custom root win
  ~SmpWindow() override { }

  void SetBackgroundColor(unsigned long);

  void Clear() override;
  void Redraw() override { }
  void Exposed() override { }
  
private:
  int   scr_;
  GC    context_;

}; // class SmpWindow

}  // namespace anshub

#endif  // IO_SMP_WINDOW_H