// *************************************************************
// File:    fx_polygons.h
// Descr:   polygon helpers functions 
// Author:  Novoselov Anton @ 2017
// URL:     https://github.com/ans-hub/geomath_lib
// *************************************************************

#ifndef SMPL_FX_POLYGONS_H
#define SMPL_FX_POLYGONS_H

#include <vector>

#include "polygon.h"
#include "line.h"
#include "exceptions.h"
#include "helpers.h"
#include "segment.h"
#include "fx_lines.h"

namespace anshub {

namespace polygon {

  void    CheckInvariant(const Vertexes&);
  double  Square(const Vertexes&);
  Point   Barycenter(const Vertexes&, BSquare);
  bool    PointInside(const Vertexes&, const Point&);
  bool    PointInside(double, double, double, double, double, double);
  bool    PointInside(const Point&, const Point&, const Point&);
  bool    PointsInside(double, double, double, double, std::vector<Point>&);
  bool    PointsInside(double, double, double, double, std::vector<Point>&&);
  bool    CutConvex(Vertexes&, Vertexes&, const Line&);

} // namespace polygon

} // namespace anshub

#endif  // SMPL_FX_POLYGONS_H