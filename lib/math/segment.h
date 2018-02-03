// *************************************************************
// File:    segment.h
// Descr:   represents 2d segment entity
// Author:  Novoselov Anton @ 2017-2018
// URL:     https://github.com/ans-hub/geomath_lib
// *************************************************************

#ifndef GM_SEGMENT_2D_H
#define GM_SEGMENT_2D_H

#include <algorithm>

#include "point.h"

namespace anshub {

struct Segment
{
  Segment() : a{}, b{} { }
  Segment(const Point& pa, const Point& pb) : a{pa}, b{pb} { }
  Point a;
  Point b;

}; // struct Segment

namespace segment2d {

  Point Divpoint(const Segment&, float);
  bool  Clip(int, int, int, int, int&, int&, int&, int&);
  bool  Clip(int, int, int, int, float&, float&, float&, float&);

} // namespace segment2d

} // namespace anshub

#endif  // GM_SEGMENT_2D_H