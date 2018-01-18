// *************************************************************
// File:    fx_lines.h
// Descr:   helper functions to lines, segments and vectors 
// Author:  Novoselov Anton @ 2017
// URL:     https://github.com/ans-hub/geomath_lib
// *************************************************************

#ifndef SMPL_FXLINES_H
#define SMPL_FXLINES_H

#include <cmath>
#include <algorithm>

#include "point.h"
#include "line.h"
#include "segment.h"
#include "vector.h"
#include "fx_vectors.h"
#include "constants.h"
#include "parmline.h"
#include "helpers.h"
#include "math.h"

namespace anshub {

namespace line {

  using BRatio = double;
  using BXcoord = double;
  using BYcoord = double;

  // Returns equation of line by two given points
  Line    Equation(const Point&, const Point&);

  // Returns equation of line by given point and perpendicular vector
  Line    Equation(const Point&, const Vec2d&);

  // Returns equation of line by given vector
  Line    Equation(const Vec2d&);

  // Divides Segment by ratio and returns divpoint
  Point   Divpoint(const Segment&, BRatio);

  // Returns perpendicular line to given line in given point 
  Line    Perpendicular(const Line&, const Point&);

  // Returns perpendicular line to given segment in given point
  Line    Perpendicular(const Segment&, const Point&);

  // Checks segments intersection and refresh intersection point
  bool    Intersects(const Segment&, const Segment&, Point&);

  // Checks lines intersection and refresh intersection point  
  bool    Intersects(const Line&, const Line&, Point&);

  // Checks line and segment intersection and refresh intersection point  
  bool    Intersects(const Line&, const Segment&, Point&);

  // Checks if point is lies on the line
  bool    Lies_on(const Line&, const Point);

} // namespace line

} // namespace anshub

#endif  // SMPL_FXLINES_H