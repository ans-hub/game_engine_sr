//***********************************************************************
// File:    gl_triangle.h
// Descr:   triangle (drawable) struct for renderer
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
//***********************************************************************

#ifndef GC_GL_TRIANGLE_H
#define GC_GL_TRIANGLE_H

#include <vector>
#include <array>

#include "gl_aliases.h"
#include "../math/vector.h"

namespace anshub {

//***********************************************************************
// Triangle struct
//***********************************************************************

// Represents triangle

struct Triangle
{
  enum Attrs {
    SSIDE         = 0,
    DSIDE         = 1,
    FLAT_SHAD     = 1 << 1,
    PHONG_SHAD    = 1 << 2,
    GOURANG_SHAD  = 1 << 3,
    HIDDEN        = 1 << 4
  };

  Triangle(Vertexes&, int, int, int, uint, uint);
  Triangle(const Vector&, const Vector&, const Vector&, uint, uint);
  
  std::array<Vector, 3> vxs_;
  std::array<int,3>     indicies_;      // see note #1 after code
  unsigned int          color_;
  unsigned int          attrs_;

}; // struct Triangle

}  // namespace anshub


#endif  // GC_GL_TRIANGLE_H

//***********************************************************************

// Note #1 : vertexes orientation is prefered as "left-handed" and
// from low index to higher. This is necessary in clipping reasons,
// when we define is polygon visible or not.
//
// For example:
//
//        p1
//  p0
//        p2
//
// V = p1-p0, P = p2-p0; N = VxN (cross prod) (normal to V and P).
// As result N would be directed from camera. Now to decide is surface
// is visible or not, we make cross (or dot) prod of N and G (surface to cam) 
