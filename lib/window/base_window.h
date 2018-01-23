// *************************************************************
// File:    base_window.h
// Descr:   pure virtual class represents `window` concept in x
// Author:  Novoselov Anton @ 2017
// URL:     https://github.com/ans-hub/iogame_lib
// *************************************************************

#include "unistd.h"

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/extensions/Xrandr.h>

#include "enums.h"
#include "aliases.h"
#include "exceptions.h"
#include "helpers.h"

#ifndef IO_BASE_WINDOW_H
#define IO_BASE_WINDOW_H

namespace anshub {

class BaseWindow
{
public:
  BaseWindow();
  BaseWindow(const BaseWindow&) =delete;
  BaseWindow(BaseWindow&&) =delete;       // todo : realise move ctor
  virtual ~BaseWindow();

  // Output hanlders

  void  Show();
  void  Hide();
  void  Move(int, int);
  void  Close();
  void  HideCursor();
  void  UnhideCursor();
  void  SetFocus();
  bool  ToggleFullscreen();
  bool  ToggleFullscreen(int);
  void  ToggleOnTop();
  bool  Closed();
  
  // Renderers

  void  Render();                 // should be called each time
  virtual void Clear() =0;        // how to clear window  
  virtual void Redraw()  =0;      // how to redraw window
  virtual void Exposed() =0;      // what to do when exposed

  // Input handlers

  auto GetNextEvent();
  Btn  ReadKeyboardBtn(BtnType);   // read keyboard and mouse
  Btn  ReadMouseBtn(BtnType);      // event in event loop by
  Pos  ReadMousePos();             // async
  
  // Helper member functions

  int   Width() const { return width_; }
  int   Height() const { return height_; }
  auto  Disp() { return disp_; }
  auto  Self() { return self_; }

protected:
  Display*    disp_;              // main display connection
  Window      root_;              // root window of display
  Window      self_;              // current window itself in terms of xlib
  XEvent      event_;             // struct to catch events
  bool        fullscreen_;        // fullscreen state
  int         vmode_;             // curr videomode in terms of Xrandr
  int         width_;             // current window width
  int         height_;            // current window height

  void NotifyWhenClose();         // see note #1
  
private:
  Atom wm_protocols_;             // necessary to catch close
  Atom wm_delete_window_;         // button event of window_manager

}; // class BaseWindow

}  // namespace anshub

#endif  // IO_BASE_WINDOW_H

// Note #1 : every child should call this in constructor if it
// wants to recieve notifies about closing window. It is necessary
// do in child class since WM notify about closing concrete window.
// We can`t call its in the BaseWindow, since self_ is not created
// yet