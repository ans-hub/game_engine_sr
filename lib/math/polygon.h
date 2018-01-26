// *************************************************************
// File:    polygon.h
// Descr:   represents polygon 2d entity and helpers
// Author:  Novoselov Anton @ 2017-2018
// URL:     https://github.com/ans-hub/geomath_lib
// *************************************************************

#ifndef GM_POLYGON_2D_H
#define GM_POLYGON_2D_H

#include <vector>

#include "point.h"
#include "line.h"
#include "math.h"
#include "helpers.h"

namespace anshub {

using Polygon = std::vector<Point>;

namespace polygon2d {

  void    CheckInvariant(const Polygon&);
  double  Square(const Polygon&);
  Point   Barycenter(const Polygon&, double);
  bool    PointInside(const Polygon&, const Point&);
  bool    PointInside(double, double, double, double, double, double);
  bool    PointInside(const Point&, const Point&, const Point&);
  bool    PointsInside(double, double, double, double, std::vector<Point>&);
  bool    PointsInside(double, double, double, double, std::vector<Point>&&);
  bool    CutConvex(Polygon&, Polygon&, const Line&);
  void    RotatePoint(double&, double&, double, math::Table&, math::Table&);

} // namespace polygon2d

} // namespace anshub

#endif  // GM_POLYGON_2D_H