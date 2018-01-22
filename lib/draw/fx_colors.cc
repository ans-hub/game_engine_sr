// ***********************************************************************
// File:    fx_colors.h
// Descr:   helpers color functions 
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
//***********************************************************************

#include <fx_colors.h>

namespace anshub {

// Returns color in word-order ARGB format (little-endian) where the most
// signification byte is b, and less is a

int color::MakeARGB(byte a, byte r, byte g, byte b)
{
  return (b << 24) | (g << 16) | (r << 8) | a;
}

// Split ARGB byte-ordered little-endian into r,g,b,a components

void color::SplitARGB(int color, byte& b, byte& g, byte& r, byte& a)
{
  b = (color >> 24) & 0xff;
  g = (color >> 16) & 0xff;
  r = (color >> 8)  & 0xff;
  a = color & 0xff;
}

// Increase brightness of rgba color (given in word-order)

int color::IncreaseBrightness(int color, double k)
{
  unsigned int a = 255;
  unsigned int r = (int)(((color>>8)&0xff)*k);
  unsigned int g = (int)(((color>>16)&0xff)*k);
  unsigned int b = (int)(((color>>24)&0xff)*k);
  if (r > 255) r = 255;
  if (g > 255) g = 255;
  if (b > 255) b = 255;
  return (b << 24) | (g << 16) | (r << 8) | a;
}

} // namespace anshub

// Note : all colors are ARGB (word-ordered). This means that on the little-endian
// architecture all ARGB colors stored in memory right to left - b,g,r,a).
// But in glDrawPixels opengl function we set type of pixel color as GL_BGRA. This is
// all right since it is equivalent: ARGB (word-ordered) and BGRA (byte-ordered).
// glDrawPixels interprets pixel not as 4 byte integer but as sequence of 4 bytes.

// https://en.wikipedia.org/wiki/RGBA_color_space
// http://www.laurenscorijn.com/articles/colormath-basics