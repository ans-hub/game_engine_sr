// *************************************************************
// File:    base_window.cc
// Descr:   pure virtual class represents `window` concept
// Author:  Novoselov Anton @ 2017
// URL:     https://github.com/ans-hub/iogame_lib
// *************************************************************

// Note : all big chunk of pure low-level code placed in ll_helpers
// due to clear understand the principles of this class without details 

#include "base_window.h"

namespace anshub {

BaseWindow::BaseWindow()
  : disp_{ XOpenDisplay(NULL) }
  , root_{}
  , self_{}
  , event_{}
  , fullscreen_{false}
  , vmode_{-1}
  , width_{0}
  , height_{0}
{
  if (!disp_)
    throw IOException("XOpenDisplay failed", errno);
  root_ = XDefaultRootWindow(disp_);

  // Ask WM for notify us when user request closing the window

  wm_protocols_     = XInternAtom(disp_, "WM_PROTOCOLS", false);
  wm_delete_window_ = XInternAtom(disp_, "WM_DELETE_WINDOW", false);
}

BaseWindow::~BaseWindow()
{
  if (vmode_ != -1)
    io_helpers::ChangeVideoMode(disp_, root_, vmode_);
  if (self_) {
    XDestroyWindow(disp_, self_);
    XFlush(disp_);
  }
  if (disp_)
    XCloseDisplay(disp_);
}
  
//********************************************************************
// OUTPUT HANDLERS
//********************************************************************

void BaseWindow::Show()
{
  XMapWindow(disp_, self_);
  XFlush(disp_);
}

void BaseWindow::Hide()
{
  XUnmapWindow(disp_, self_);
  XFlush(disp_);
}

void BaseWindow::Move(int x, int y)
{
  XMoveWindow(disp_, self_, x, y);
}

void BaseWindow::Close()
{
  io_helpers::SendCloseWindowNotify(disp_, root_, self_);
}

void BaseWindow::HideCursor()
{
  io_helpers::HideCursor(disp_, self_);
}

void BaseWindow::UnhideCursor()
{
  io_helpers::UnhideCursor(disp_, self_);
}

void BaseWindow::SetFocus()
{
  XRaiseWindow(disp_, self_);
  XSetInputFocus(disp_, self_, RevertToNone, CurrentTime);
}

bool BaseWindow::ToggleFullscreen()
{
  int curr = io_helpers::GetCurrentVideoMode(disp_, root_);
  return this->ToggleFullscreen(curr);  
}

bool BaseWindow::ToggleFullscreen(int mode)
{
  if (mode < 0)
    return false;

  this->Move(0,0);
  vmode_ = io_helpers::ChangeVideoMode(disp_, root_, mode);
  io_helpers::GetWindowDimension(disp_, root_, &width_, &height_);

  int wait = 1000;
  do {                  // wait when window is shown (usually after
                        // changing resolution) this is no smart
    timespec ts;        
    ts.tv_sec  = wait / 1000;
    ts.tv_nsec = wait * 1000000;
    while ((nanosleep(&ts, &ts) == -1) && (errno == EINTR)) { }

  } while (GrabSuccess != XGrabPointer(
        disp_, self_, True, None, GrabModeAsync, GrabModeAsync,
        self_, None, CurrentTime));
  SetFocus();
  XWarpPointer(disp_, None, root_, 0, 0, 0, 0, 0, 0); // todo: place in the middle of the scr

  bool result = io_helpers::SendToggleFullscreenNotify(disp_, root_, self_);
  fullscreen_ ^= result;

  if (!fullscreen_)
    XUngrabPointer(disp_, CurrentTime);
  
  return true;

  // 1: the last thing is to catch VisibilityEvent and raise window (see Render());
  // 2: video mode changes back in destructor
}

void BaseWindow::ToggleOnTop()
{
  io_helpers::SendToggleOnTopNotify(disp_, root_, self_);
}

// Check if we have event of closing the window by user

bool BaseWindow::Closed()
{
  if (XCheckTypedWindowEvent(disp_, self_, ClientMessage, &event_)) {
    if (event_.xclient.message_type == wm_protocols_ &&
        event_.xclient.data.l[0] == (int)wm_delete_window_) {
      return true;
    }        
  }
  return false;
}

//********************************************************************
// RENDERERS
//********************************************************************

// This function called each time in loop, and then calls virtuals:
//  Exposed() - only if window was moved, overlapperd, etc
//  Up window (need to hide wm ontop elements after change resolution)
//  Redraw()  - every time

void BaseWindow::Render()
{
  if (XCheckWindowEvent(disp_, self_, ExposureMask, &event_)) {
    Exposed();    // virtual
    io_helpers::GetWindowDimension(disp_, self_, &width_, &height_);
  }
  if (fullscreen_ && XCheckTypedWindowEvent(disp_, self_, VisibilityNotify, &event_)) {
    XRaiseWindow(disp_, self_);
    io_helpers::GetWindowDimension(disp_, self_, &width_, &height_);
  }
  Redraw();       // virtual
}

//********************************************************************
// INPUT HANDLERS
//********************************************************************

// Wait for next event and returns it

auto BaseWindow::GetNextEvent()
{
  XNextEvent(disp_, &event_);
  switch (event_.type) {
    case Expose         : return WinEvent::EXPOSE;
    case KeyPress       : return WinEvent::KEYPRESS;
    case KeyRelease     : return WinEvent::KEYRELEASE;
    case ButtonPress    : return WinEvent::MOUSEPRESS;
    case ButtonRelease  : return WinEvent::MOUSERELEASE;
    case MotionNotify   : return WinEvent::MOUSEMOVE;
    default             : return WinEvent::NONSENCE;
  }
}

Btn BaseWindow::ReadKeyboardBtn(BtnType t) const      // see note #1 
{
  XEvent event;
  auto buf = Btn::NONE;
  long type = 1L << static_cast<int>(t);              // see note #2
  if (XCheckWindowEvent(disp_, self_, type, &event)) // see note #3
  {
    auto key = XkbKeycodeToKeysym(disp_, event.xkey.keycode, 0, 0);
    char buff[20];
    XLookupString(&event.xkey, buff, 20, &key, NULL);  // see note below
    buf = static_cast<Btn>(key);
  }
  return buf;

  // Note : This string is necessary if somewho press key in layout
  // differ than ISO Latin-1
}

Btn BaseWindow::ReadMouseBtn(BtnType t) const
{
  XEvent event;
  auto buf = Btn::NONE;
  long type = 1L << static_cast<int>(t);              // see note #2  
  if (XCheckWindowEvent(disp_, self_, type, &event)) // see note #3
  {
    buf = static_cast<Btn>(event.xbutton.button + kMouseBtnOffset);
  }
  return buf;
}

Pos BaseWindow::ReadMousePos() const
{
  Window root_ret;
  Window child_ret;
  int x_rel {0};
  int y_rel {0};
  int x_win {0};
  int y_win {0};
  unsigned int mask;

  XQueryPointer(
    disp_, self_, &root_ret, &child_ret, &x_rel, &y_rel, &x_win, &y_win, &mask);
  return Pos(x_win, y_win);

  // Variants - http://www.rahul.net/kenton/perf.html
  
  // Pos buf {};
  // if (XCheckWindowEvent(disp_, self_, PointerMotionMask, &event_))
  // {
  //   XSync(disp_, true);
  //   buf.x = event_.xmotion.x;
  //   buf.y = event_.xmotion.y;
  // }
  // return buf;
}

// From here: https://goo.gl/W3zqgh

bool BaseWindow::IsKeyboardBtnPressed(KbdBtn btn) const
{
  if (btn == KbdBtn::NONE)
    return false;

  KeyCode keycode = XKeysymToKeycode(disp_, static_cast<KeySym>(btn));
  
  if (keycode) {
    char keys[32];
    XQueryKeymap(disp_, keys);
    return (keys[keycode / 8] & (1 << (keycode % 8))) != 0;
  }
  else
    return false;
}

// From here: https://goo.gl/W3zqgh

bool BaseWindow::IsMouseBtnPressed(MouseBtn btn) const
{
  Window root_ret;
  Window child_ret;
  int x_rel {0};
  int y_rel {0};
  int x_win {0};
  int y_win {0};
  unsigned int btns;

  XQueryPointer(
    disp_, self_, &root_ret, &child_ret, &x_rel, &y_rel, &x_win, &y_win, &btns);

  switch (btn)
  {
    case MouseBtn::LMB:     return btns & Button1Mask;
    case MouseBtn::RMB:     return btns & Button3Mask;
    case MouseBtn::MMB:     return btns & Button2Mask;
    case MouseBtn::WH_UP:   return false;
    case MouseBtn::WH_DWN:  return false;
    default:                return false;
  }
  return false;
}

//********************************************************************
// PRIVATE IMPLEMENTATION
//********************************************************************

// Ask WM to notify when it should close the window

void BaseWindow::NotifyWhenClose()
{
  XSetWMProtocols(disp_, self_, &wm_delete_window_, 1);
}

} // namespace anshub

// Note #3 : alternative way is to query is button is pressed or not

// Note #2 : if you ever seen what is event_mask is, you should understand
// what is it

// Note #3 : XCheckWindowEvent don't wait for next event (like XNextEvent)
// but check if event is present. This function only works with masked
// events. For not masked events you may use XCheckTypedWindowEvent()