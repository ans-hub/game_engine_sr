// *************************************************************
// File:    polygon.h
// Descr:   represents polygon entitie
// Author:  Novoselov Anton @ 2017
// URL:     https://github.com/ans-hub/geomath_lib
// *************************************************************

#ifndef SMPL_POLYGON_H
#define SMPL_POLYGON_H

#include <vector>

#include "point.h"

namespace anshub {

using Vertex = Point;
using Vertexes = std::vector<Vertex>;
using BSquare = double; // prefix B means alias for base type

struct Polygon
{
  Vertexes elem;

}; // struct Polygon

}  // namespace anshub

#endif  // SMPL_POLYGON_H