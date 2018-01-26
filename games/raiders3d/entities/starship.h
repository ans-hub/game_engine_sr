// *************************************************************
// File:    starship.h
// Descr:   starship entity
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef GAME_STARSHIP_H
#define GAME_STARSHIP_H

#include <vector>

#include "config.h"
#include "lib/math/vector.h"
#include "lib/math/segment.h"

#include "lib/math/point.h"
#include "lib/math/segment.h"

namespace anshub {

// Represent edge in Vertexes meaning

struct Edge
{
  Edge() : v1{-1.0}, v2{-1.0} { }
  Edge(double a, double b) : v1{a}, v2{b} { }
  double v1;  // number of vertex 1
  double v2;  // number of vertex 2

}; // struct Edge

// Represents starship entity

struct Starship
{
  using Vertexes = std::vector<Point>;  // n points
  using Edges = std::vector<Edge>;      // two number of points
  using Rect = Segment;                 // simple left bottom and right top 

  Starship();

  Point     pos_;           // starship global pos
  Vector    vel_;           // starship velocity
  int       color_;         // starship color
  Vertexes  vx_;            // model vertexes (local coords)
  Edges     ed_;            // model edges (pair of indexes in vx_)
  Rect      bounding_box_;  // used for laser hit detection 
  
  // States

  bool      dead_;          // is dead ?
  bool      audible_;       // is audible for current viewpoint ?
  int       attack_seq_;    // how much shots should do this ship
  int       attack_wait_;   // times between shots in sequence

}; // struct Starship

}  // namespace anshub

#endif  // GAME_STARSHIP_H