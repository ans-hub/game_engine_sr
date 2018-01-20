// *************************************************************
// File:    player.h
// Descr:   player (viewport) entitie
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef GAME_PLAYER_H
#define GAME_PLAYER_H

#include "src/config.h"

namespace anshub {

struct Player
{
  Player() 
    : w_(40), h_(40)
    , pos_(0,0,0)
    , life_{cfg::kPlayerLife}
    , dead_{false}
    , bang_{false}
    , shaked_{false}
    , curr_angle_{0}
    , offset_angle_{0}
  { } 

  double Angle() const { return curr_angle_ + offset_angle_; }

  int w_;          // player width
  int h_;          // player height
  Point pos_;      // player pos
  int   life_;     // player life 
  bool dead_;
  bool bang_;
  bool shaked_;
  bool side_;
  double curr_angle_;   // rotate angle
  double offset_angle_;

}; // struct Player

}  // namespace anshub

#endif  //  GAME_PLAYER_H