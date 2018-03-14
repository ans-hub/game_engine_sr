// *************************************************************
// File:    terrain.h
// Descr:   generates terrain
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef GL_EXTRAS_TERRAIN_H
#define GL_EXTRAS_TERRAIN_H

#include "../gl_object.h"

namespace anshub {

// Class represents textured terrain which is generated from highmap

struct Terrain : public GlObject
{
  Terrain(const char* map_fname, const char* tex_fname, int div);

}; // struct Terrain

}  // namespace anshub

#endif // GL_EXTRAS_TERRAIN_H