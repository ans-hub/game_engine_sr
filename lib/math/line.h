// *************************************************************
// File:    line.h
// Descr:   represents line
// Author:  Novoselov Anton @ 2017
// URL:     https://github.com/ans-hub/geomath_lib
// *************************************************************

#ifndef SMPL_LINE_H
#define SMPL_LINE_H

#include "point.h"

namespace anshub {

struct Line
{
  Line() : a{1}, b{1}, c{0} { } // invariant (a || b != 0)
  Line(double pa, double pb, double pc) : a{pa}, b{pb}, c{pc} { }
  double a;
  double b;
  double c;
  double GetX(double y) const { return ((-b*y)-c) / a; }
  double GetY(double x) const { return ((-a*x)-c) / b; }

}; // struct Line

}  // namespace anshub

#endif  // SMPL_LINE_H