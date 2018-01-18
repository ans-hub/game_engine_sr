// *************************************************************
// File:    level.cc
// Descr:   contains game object and state
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#include "level.h"

namespace anshub {

Level::Level(int ships)
  : ships_(ships)
  , stars_(cfg::kStarsCnt)
  , cannon_{}
  , explosions_{}
  , score_{0}
  , velocity_{cfg::kVelocity}
{ }

} // namespace anshub