// *************************************************************
// File:    water.h
// Descr:   represents very simple water
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef GL_EXTRAS_WATER_H
#define GL_EXTRAS_WATER_H

#include "lib/draw/exceptions.h"
#include "lib/draw/gl_aliases.h"
#include "lib/draw/gl_object.h" 
#include "lib/draw/fx_colors.h"
#include "lib/draw/gl_vertex.h"

namespace anshub {

//*************************************************************************
// Very simple water
//*************************************************************************

struct Water : public GlObject
{
  Water(int width, float height, cFColor&, Shading);

private:
  int     width_;
  float   height_;
  FColor  color_;

}; // struct Water

}  // namespace anshub

#endif // GL_EXTRAS_WATER_H