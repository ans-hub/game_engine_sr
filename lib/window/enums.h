// *************************************************************
// File:    enums.h
// Descr:   enumerators and helper structs
// Author:  Novoselov Anton @ 2017
// *************************************************************

#ifndef IO_ENUMS_H
#define IO_ENUMS_H

#define XK_LATIN1         // for X11/keysymdef.h
#define XK_MISCELLANY     // the same as above

#include <X11/keysymdef.h>

namespace anshub {

// Represents i/o event of windows 

enum class WinEvent
{
  EXPOSE,
  KEYPRESS,
  KEYRELEASE,
  MOUSEPRESS,
  MOUSERELEASE,
  MOUSEMOVE,
  NONSENCE

}; // enum class WinEvent

// Represents button event types

enum class BtnType
{
  KB_DOWN = 0,    // keyboard key down
  KB_UP,          // keyboard key up
  MS_DOWN,        // mouse key down
  MS_UP           // mouse key up

}; // enum class BtnType

constexpr int kMouseBtnOffset = 1000;   // see below

// Represents pressed (released) button code

enum class Btn
{
  NONE    = 0,
  SPACE   = 32,
  NUM0    = 48, NUM1, NUM2, NUM3, NUM4, NUM5, NUM6, NUM7, NUM8, NUM9,
  A       = 97, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T,
            U, V, W, X, Y, Z,
  ENTER   = 65293,
  LEFT    = 65361, UP, RIGHT, DOWN,
  ESC     = 65307,
  RSHIFT  = 65505, LSHIFT,
  LMB     = kMouseBtnOffset + 1, 
            MMB, RMB, WH_UP, WH_DWN

}; // enum class Btn

// Represents keyboard buttons to another way of getting

enum class KbdBtn
{
  NONE   = 0,
  LSHIFT = XK_Shift_L,
  RSHIFT = XK_Shift_R,
  LCTRL  = XK_Control_L,
  RCTRL  = XK_Control_R,
  LALT   = XK_Alt_L,
  RALT   = XK_Alt_R,
  LSUPER = XK_Super_L,
  RSUPER = XK_Super_R,
  ESC    = XK_Escape,
  SPACE  = XK_space,
  ENTER  = XK_Return,
  BACKS  = XK_BackSpace,
  TAB    = XK_Tab,
  F1     = XK_F1,
  F2     = XK_F2,
  F3     = XK_F3,
  F4     = XK_F4, 
  F5     = XK_F5, 
  F6     = XK_F6, 
  F7     = XK_F7, 
  F8     = XK_F8, 
  F9     = XK_F9, 
  F10    = XK_F10, 
  F11    = XK_F11, 
  F12    = XK_F12, 
  LEFT   = XK_Left, 
  RIGHT  = XK_Right, 
  UP     = XK_Up, 
  DOWN   = XK_Down, 
  A      = XK_a,
  B      = XK_b,
  C      = XK_c,
  D      = XK_d,
  E      = XK_e,
  F      = XK_f,
  G      = XK_g,
  H      = XK_h,
  I      = XK_i,
  J      = XK_j,
  K      = XK_k,
  L      = XK_l,
  M      = XK_m,
  N      = XK_n,
  O      = XK_o,
  P      = XK_p,
  Q      = XK_q,
  R      = XK_r,
  S      = XK_s,
  T      = XK_t,
  U      = XK_u,
  V      = XK_v,
  W      = XK_w,
  X      = XK_x,
  Y      = XK_y,
  Z      = XK_z,
  NUM0   = XK_0,
  NUM1   = XK_1,
  NUM2   = XK_2,
  NUM3   = XK_3,
  NUM4   = XK_4,
  NUM5   = XK_5,
  NUM6   = XK_6,
  NUM7   = XK_7,
  NUM8   = XK_8,
  NUM9   = XK_9

}; // enum class KbdBtn

// Represents mouse buttons

enum class MouseBtn
{
  NONE = 0,
  LMB, 
  MMB,
  RMB,
  WH_UP,
  WH_DWN

}; // enum class MouseBtn

// Represents position of something on the screen

struct Pos
{
  Pos() : x{-1}, y{-1} { }
  Pos(int ux, int uy) : x{ux}, y{uy} { } 
  int x; 
  int y;

}; // struct Pos

// Represents size of something that has width and height

struct Size
{
  Size() : w{-1}, h{-1} { }
  Size(int uw, int uh) : w{uw}, h{uh} { }
  int w; 
  int h; 

}; // struct Size

// Represents gl context profile

enum class GlProfile
{
  UNKNOWN = 0,
  CORE,
  COMP
  
}; // enum class GlProfile

} // namespace anshub

#endif  // IO_ENUMS_H