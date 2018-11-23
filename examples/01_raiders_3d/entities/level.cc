// *************************************************************
// File:    level.cc
// Descr:   contains game object and state
// Author:  Novoselov Anton @ 2017
// *************************************************************

#include "level.h"

namespace anshub {

Level::Level(int ships)
  : ships_(ships)
  , stars_(cfg::kStarsCnt)
  , cannon_{}
  , explosions_{}
  , enemy_shots_{}
  , player_{}
  , state_{GameState::PLAY}
  , score_{0}
  , ships_destroyed_{0}
{ }

} // namespace anshub