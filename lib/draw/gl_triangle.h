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

#include "../math/vector.h"

namespace anshub {

using Vertexes  = std::vector<Vector>;
using Edges     = std::array<int,  3>;
using EdgesP    = std::array<Vector,3>;

//***********************************************************************
// Triangle struct
//***********************************************************************

// Represents triangle with reference to vertexes

struct Triangle
{
  using uint = unsigned int;

  enum Attrs {
    SSIDE         = 0,
    DSIDE         = 1,
    FLAT_SHAD     = 1 << 1,
    PHONG_SHAD    = 1 << 2,
    GOURANG_SHAD  = 1 << 3,
    HIDDEN        = 1 << 4
  };

  Triangle(Vertexes&, int, int, int, uint, uint);
  
  std::vector<Vector>&  vxs_;           // see note #1 after code
  std::array<int,3>     indicies_;      // see note #2 after code
  unsigned int          color_;
  unsigned int          attrs_;

}; // struct Triangle


//***********************************************************************
// Triangle face struct
//***********************************************************************

// Represents triange and stores point inside struct

struct TriangleFace
{
  TriangleFace(const Vector&, const Vector&, const Vector&, uint, uint);

  std::array<Vector,3>  vxs_;
  std::array<Vector,3>  curr_;
  unsigned int          color_;
  unsigned int          attrs_;

}; // struct TriangleFace

}  // namespace anshub

#endif  // GC_GL_TRIANGLE_H

//***********************************************************************

// Note #1 : we use indexes to vertexes but not vertexes itself due
// to memory economy and flexibility. For example, in this scheme we
// can make any changes with this polygon and not touch real points.
// Additional reason - is floating point error, which would be accumulated
// in object transformation (each vertex may be vertex of not only one
// polygon)

// Note #2 : vertexes orientation is prefered as "left-handed" and
// from low index to higher. This is necessary in clipping reasons,
// when we define is polygon visible or not.
//
// For example:
//        p1
//  p0
//        p2
// V = p1-p0, P = p2-p0; N = VxN (cross prod) (normal to V and P).
// As result N would be directed from camera. Now to decide is surface
// is visible or not, we make cross (or dot) prod of N and G (surface to cam) 
