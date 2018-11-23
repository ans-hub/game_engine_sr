// *************************************************************
// File:    water.h
// Descr:   represents very simple water-plane
// Author:  Novoselov Anton @ 2017
// *************************************************************

#ifndef GL_EXTRAS_WATER_H
#define GL_EXTRAS_WATER_H

#include "lib/render/exceptions.h"
#include "lib/render/gl_aliases.h"
#include "lib/render/gl_object.h" 
#include "lib/render/fx_colors.h"
#include "lib/render/gl_vertex.h"

namespace anshub {

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