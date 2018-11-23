// *************************************************************
// File:    level.h
// Descr:   contains game object and state
// Author:  Novoselov Anton @ 2018
// *************************************************************

#ifndef GAME_LEVEL_H
#define GAME_LEVEL_H

#include <vector>

#include "config.h"
#include "lib/math/vector.h"
#include "lib/math/segment.h"

#include "entities/enums.h"
#include "entities/game_objects.h"
#include "entities/game_objects.h"

namespace anshub {

struct Level
{
  using VBall  = std::vector<Ball>;
  using VBlock = std::vector<Block>;

  Level(int balls);

  // Game objects

  VBall         balls_;
  VBlock        blocks_;
  Paddle        paddle_;
  GameState     state_;

}; // struct Level

}  // namespace anshub

#endif  // GAME_LEVEL_H