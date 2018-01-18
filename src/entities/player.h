// *************************************************************
// File:    player.h
// Descr:   player (viewport) entitie
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef GAME_PLAYER_H
#define GAME_PLAYER_H

namespace anshub {

struct Player
{
  bool shaked_;
  bool side_;
  int x_offset_;
  int y_offset_;

}; // struct Player

}  // namespace anshub

#endif  //  GAME_PLAYER_H