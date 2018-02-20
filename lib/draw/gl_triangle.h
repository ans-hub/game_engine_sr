//***********************************************************************
// File:    gl_triangle.h
// Descr:   drawable triangle struct for renderer
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
//***********************************************************************

#ifndef GC_GL_TRIANGLE_H
#define GC_GL_TRIANGLE_H

#include <vector>
#include <array>

#include "gl_aliases.h"
#include "fx_colors.h"
#include "../math/vector.h"

namespace anshub {

//***********************************************************************
// Triangle struct
//***********************************************************************

// Represents drawable triangle

struct Triangle
{
  enum Attrs {
    VISIBLE           = 0,
    HIDDEN            = 1,
    CONST_SHADING     = 1 << 1,
    FLAT_SHADING      = 1 << 2,
    PHONG_SHADING     = 1 << 3,
    GOURANG_SHADING   = 1 << 4
  };

  // Constructs triangle just with indicies && attributes

  Triangle(int v1, int v2, int v3, uint attrs);
  
  // Constructs triangle with self contained vertexes and colors

  Triangle(
    cVector& v1, cVector& v2, cVector& v3, cColor& c1, cColor& c2, cColor& c3,
    uint attrs
  );

  // Vector& v1;  // reference to vertexes
  // Vector& v2;
  // Vector& v3;
  // Color&  c1;  // reference to colors_
  
  std::array<Vector,3>  vxs_;       // independent vertexes 
  std::array<Color<>,3> colors_;    // independent colors 
  std::array<int,3>     indicies_;  // indexed to vertexes in GlObject
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