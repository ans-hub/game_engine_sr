// *************************************************************
// File:    fx_colors.h
// Descr:   helpers color functions 
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef GL_FXCOLORS_H
#define GL_FXCOLORS_H

#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "gl_aliases.h"

namespace anshub {

// Simple struct represents color

struct Color
{
  Color() : r{}, g{}, b{}, a{} { }
  Color(uchar cr, uchar cg, uchar cb) : r{cr}, g{cg}, b{cb}, a{} { }
  uint GetARGB() const { return (b << 24) | (g << 16) | (r << 8) | a; }
  uchar r;
  uchar g;
  uchar b;
  uchar a;

}; // struct Color

namespace color {

// Const colors

constexpr uint White {0xffffffff};
constexpr uint Black {0x00000000};

// Helpers functions

int   MakeARGB(byte a, byte r, byte g, byte b);
void  SplitARGB(int color, byte& b, byte& g, byte& r, byte& a);
int   IncreaseBrightness(int color, float k);

} // namespace color

// Implementation of inline functions

// Returns color in word-order ARGB format (little-endian) where the most
// signification byte is b, and less is a

inline int color::MakeARGB(byte a, byte r, byte g, byte b)
{
  return (b << 24) | (g << 16) | (r << 8) | a;
}

} // namespace anshub

#endif  // GL_FXCOLORS_H