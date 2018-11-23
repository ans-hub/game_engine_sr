// *************************************************************
// File:    cannon.h
// Descr:   cannon entity
// Author:  Novoselov Anton @ 2017
// *************************************************************

#ifndef GAME_CANNON_H
#define GAME_CANNON_H

#include "lib/math/vector.h"

namespace anshub {

struct Cannon
{
  Cannon() : mid_{}, ready_{true}, shot_{false}, wait_{0} { }
  Vector  mid_;
  int     color_;
  bool    ready_;
  bool    shot_;
  int     wait_;

}; // struct Cannon

}  // namespace anshub

#endif  // GAME_CANNON_H