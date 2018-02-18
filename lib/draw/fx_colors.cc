// ***********************************************************************
// File:    fx_colors.h
// Descr:   helpers color functions 
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
//***********************************************************************

#include <fx_colors.h>

namespace anshub {

// Split ARGB byte-ordered little-endian into r,g,b,a components

void color::SplitARGB(int color, byte& b, byte& g, byte& r, byte& a)
{
  b = (color >> 24) & 0xff;
  g = (color >> 16) & 0xff;
  r = (color >> 8)  & 0xff;
  a = color & 0xff;
}

// Increase brightness of rgba color (given in word-order)

int color::IncreaseBrightness(int color, float k)
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