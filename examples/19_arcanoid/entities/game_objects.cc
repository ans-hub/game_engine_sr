// *************************************************************
// File:    game_objects.cc
// Descr:   ball, paddle and blocks entities 
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#include "game_objects.h"

namespace anshub {

Ball::Ball()
  : pos_{}
  , vel_{}
  , color_{cfg::kBallColor}
  , active_{true}
{ }

Paddle::Paddle()
  : pos_{}
  , vel_{}
  , color_{cfg::kPaddleColor}
  , active_{true}
{ }

Block::Block()
  : pos_{}
  , vel_{}
  , color_{cfg::kBlockColor}
  , active_{true}
{ }

} // namespace anshub