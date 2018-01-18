// *************************************************************
// File:    segment.h
// Descr:   represents segment entitie
// Author:  Novoselov Anton @ 2017
// URL:     https://github.com/ans-hub/geomath_lib
// *************************************************************

#ifndef SMPL_SEGMENT_H
#define SMPL_SEGMENT_H

#include "point.h"

namespace anshub {

struct Segment
{
  Segment() : a{}, b{} { }
  Segment(const Point& pa, const Point& pb) : a{pa}, b{pb} { }
  virtual ~Segment() { }
  Point a;
  Point b;

}; // struct Segment

}  // namespace anshub

#endif  // SMPL_SEGMENT_H