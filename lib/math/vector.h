// *************************************************************
// File:    vector.h
// Descr:   represents vector
// Author:  Novoselov Anton @ 2017
// URL:     https://github.com/ans-hub/geomath_lib
// *************************************************************

#ifndef GM_VECTOR_H
#define GM_VECTOR_H

#include "point.h"
#include "segment.h"

namespace anshub {

struct Vec2d : public Point
{
  Vec2d() : Point() { }
  explicit Vec2d(const Point& p)
    : Point(p) { }
  explicit Vec2d(const Segment& s)
    : Point(s.b.x - s.a.x, s.b.y - s.a.y, s.b.z - s.a.z) { }

  inline Vec2d Normalize(Vec2d&) { return Vec2d(); }
  inline void  Rotate(Vec2d&, double) { }

}; // struct Vec2d

}  // namespace anshub

#endif  // GM_VECTOR_H

// Todo : Length, Add (oper+), Sub (oper-), Dot, Cross