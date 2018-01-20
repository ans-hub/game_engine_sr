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
    : w(40), h(40)
    , pos(0,0,0)
    , life{cfg::kPlayerLife}
    , dead_{false}
    , bang_{false}
    , shaked_{false} { } 

  int w;          // player width
  int h;          // player height
  Point pos;      // player pos
  int   life;     // player life 
  bool dead_;
  bool bang_;
  bool shaked_;
  bool side_;
  int x_offset_;
  int y_offset_;

}; // struct Player

}  // namespace anshub

#endif  //  GAME_PLAYER_H