// *************************************************************
// File:    cannon.h
// Descr:   cannon entitie
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef GAME_CANNON_H
#define GAME_CANNON_H

#include "lib/math/point.h"

namespace anshub {

struct Cannon
{
  Cannon() : mid_{}, ready_{true}, shot_{false}, wait_{0} { }
  Point mid_;
  int   color_;
  bool  ready_;     // ready state
  bool  shot_;      // shot state
  int   wait_;      // wait state

}; // struct Cannon

}  // namespace anshub

#endif  // GAME_CANNON_H