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
#include <functional>

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
  enum Attrs
  {
    VISIBLE           = 0,
    HIDDEN            = 1,
    CONST_SHADING     = 1 << 1,
    FLAT_SHADING      = 1 << 2,
    PHONG_SHADING     = 1 << 3,
    GOURANG_SHADING   = 1 << 4
  };

  // Constructs triangle just with indicies && attributes

  Triangle(Vertexes&, FColors&, int f1, int f2, int f3, uint attrs);
  
  Vertexes& vxs_;
  FColors&  colors_;
  
  // This fields used only inside ::triangles namespace

  Vector  v1_;
  Vector  v2_;
  Vector  v3_;
  FColor  c1_;
  FColor  c2_;
  FColor  c3_;

  // This fields used before triangulation objects

  int     f1_;
  int     f2_;
  int     f3_;
  
  // Other usefull stuff

  Vector  face_normal_;
  uint    attrs_;

}; // struct Triangle

namespace triangle {

  void CalcFaceNormal(Triangle&, const Vertexes&);
  void CalcVertexNormal(Triangle&, const Vertexes&);

} // namespace triangle

} // namespace anshub

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