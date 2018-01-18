// *************************************************************
// File:    parmline.h
// Descr:   represents parametrized line
// Author:  Novoselov Anton @ 2017
// URL:     https://github.com/ans-hub/geomath_lib
// *************************************************************

#ifndef SMPL_PARMLINE_H
#define SMPL_PARMLINE_H

#include "point.h"
#include "vector.h"

namespace anshub {

struct ParmLine
{
  ParmLine(const Point& pa, const Point& pb)
    : a{pa}
    , b{pb}
    , v{pb - pa}
  { }
  Point a;
  Point b;
  Vec2d v;

}; // struct ParmLine

}  // namespace anshub

#endif  // SMPL_PARMLINE_H