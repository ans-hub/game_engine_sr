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

namespace color {

// Const colors

constexpr uint White {0xffffffff};
constexpr uint Black {0x00000000};

// Helpers functions

int   MakeARGB(byte, byte, byte, byte);
void  SplitARGB(int color, byte&, byte&, byte&, byte&);
int   IncreaseBrightness(int, float);

} // namespace color

} // namespace anshub

#endif  // GL_FXCOLORS_H