// *************************************************************
// File:    level.h
// Descr:   contains game object and state
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef GAME_LEVEL_H
#define GAME_LEVEL_H

#include <vector>

#include "src/config.h"
#include "lib/math/point.h"
#include "lib/math/segment.h"
#include "src/entities/starship.h"
#include "src/entities/cannon.h"
#include "src/entities/player.h"

namespace anshub {

struct Level
{
  using VStarships  = std::vector<Starship>;
  using VSegments   = std::vector<Segment>;
  using VStarfield  = std::vector<Point>;
  using VVelocity   = std::vector<Vector3d>;
  using VExplosions = std::vector<std::pair<VSegments, VVelocity>>;

  Level(int ships);

  // Game objects

  VStarships  ships_;
  VStarfield  stars_;
  Cannon      cannon_;
  VExplosions explosions_;
  Player      player_;
  
  // States

  int         score_;
  int         velocity_;

}; // struct Level

}  // namespace anshub

#endif  // GAME_LEVEL_H