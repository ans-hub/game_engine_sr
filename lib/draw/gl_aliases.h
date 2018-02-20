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
  template<class T> struct Color;
  struct GlObject;
  struct Triangle;

  using uint  = unsigned int;
  using uchar = unsigned char;
  using Vuint = std::vector<uint>;
  using Vuchar = std::vector<uchar>;
  using byte  = unsigned char;
  using FColor = Color<float>;
  using cFColor = const Color<float>;
  using cColor = const Color<uchar>;
  using Colors = std::vector<Color<uchar>>;
  using Vertexes  = std::vector<Vector>;
  using Objects   = std::vector<GlObject>;
  using Matrix2d  = std::vector<std::vector<double>>;
  using cMatrix2d = const Matrix2d;
  using Triangles     = std::vector<Triangle>;

} // namespace anshub

#endif  // GC_DRAW_ALIASES_H