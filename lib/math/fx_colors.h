// *************************************************************
// File:    fx_colors.h
// Descr:   helpers color functions 
// Author:  Novoselov Anton @ 2017
// URL:     https://github.com/ans-hub/geomath_lib
// *************************************************************

#ifndef SMPL_FXCOLORS_H
#define SMPL_FXCOLORS_H

#include <stdlib.h>
#include <unistd.h>
#include <time.h>

namespace anshub {

namespace color {

using byte  = unsigned char;

int   MakeARGB(byte, byte, byte, byte);
void  SplitARGB(int color, byte&, byte&, byte&, byte&);
int   IncreaseBrightness(int, double);

} // namespace color

} // namespace anshub

#endif  // SMPL_FXCOLORS_H