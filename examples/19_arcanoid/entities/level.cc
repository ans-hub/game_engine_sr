// *************************************************************
// File:    level.cc
// Descr:   contains game object and state
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#include "level.h"

namespace anshub {

Level::Level(int balls)
  : balls_(balls)
  , blocks_()
  , paddle_()
  , state_{GameState::PLAY}
{ }

} // namespace anshub