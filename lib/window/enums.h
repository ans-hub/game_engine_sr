// *************************************************************
// File:    enums.h
// Descr:   enumerators and helper structs
// Author:  Novoselov Anton @ 2017
// URL:     https://github.com/ans-hub/iogame_lib
// *************************************************************

#ifndef IO_ENUMS_H
#define IO_ENUMS_H

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

// Represent pressed (released) button code

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