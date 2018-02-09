// *************************************************************
// File:    line2d.h
// Descr:   represents 2d line entity on the plane
// Author:  Novoselov Anton @ 2017-2018
// URL:     https://github.com/ans-hub/geomath_lib
// *************************************************************

#ifndef GM_LINE_2D_H
#define GM_LINE_2D_H

#include "vector.h"
#include "segment.h"

namespace anshub {

// Line struct represents 2d line using general form 
//  ax + by + c = 0, where a || b (both) != 0

struct Line
{  
  float a;
  float b;
  float c;
 
  Line() : a{1}, b{1}, c{0} { }   // invariant (a || b != 0)
  Line(float pa, float pb, float pc) : a{pa}, b{pb}, c{pc} { }

  // Returns intersection point with axises
  
  float GetX(float y) const { return ((-b*y)-c) / a; }
  float GetY(float x) const { return ((-a*x)-c) / b; }

}; // struct Line

// Helpers functions definition

namespace line2d
{
  // Returns equations of 2d lines by different sources

  Line    EquationA(const Point&, const Point&);
  Line    EquationB(const Point&, const Vector&);    // normal vector
  Line    EquationC(const Vector&);                  // radius-vector
  Line    Perpendicular(const Line&, const Point&);

  // Checks segments intersection and refresh intersection point

  bool    Intersects(const Segment&, const Segment&, Point&);
  bool    Intersects(const Line&, const Line&, Point&);
  bool    Intersects(const Line&, const Segment&, Point&);

} // namespace line2d

} // namespace anshub

#endif  // GM_LINE_2D_H