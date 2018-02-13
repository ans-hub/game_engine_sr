// *************************************************************
// File:    gl_aliases.h
// Descr:   most used aliases in draw lib
// Author:  Novoselov Anton @ 2017-2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef GC_DRAW_ALIASES_H
#define GC_DRAW_ALIASES_H

#include <vector>

namespace anshub {

  struct Vector;
  struct GlObject;
  struct Triangle;

  using uint  = unsigned int;
  using Vuint = std::vector<uint>;
  using byte  = unsigned char;
  using Vertexes  = std::vector<Vector>;
  using Objects   = std::vector<GlObject>;
  using Matrix2d  = std::vector<std::vector<double>>;
  using Triangles     = std::vector<Triangle>;

} // namespace anshub

#endif  // GC_DRAW_ALIASES_H