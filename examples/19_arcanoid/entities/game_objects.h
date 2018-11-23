// *************************************************************
// File:    game_objects.h
// Descr:   ball, paddle and block entities
// Author:  Novoselov Anton @ 2018
// *************************************************************

#ifndef GAME_BALL_N_PADDLE_H
#define GAME_BALL_N_PADDLE_H

#include <vector>

#include "config.h"
#include "lib/math/vector.h"
#include "lib/math/segment.h"
#include "lib/math/segment.h"

namespace anshub {

// Represents ball entity

struct Ball
{
  Ball();

  Vector    pos_;           // ball global pos
  Vector    vel_;           // ball velocity
  int       color_;         // ball color
  bool      active_;        // is active

}; // struct Ball

// Represents paddle entity

struct Paddle
{
  Paddle();

  Vector    pos_;           // paddle global pos
  Vector    vel_;           // paddle velocity
  int       color_;         // paddle color
  bool      active_;        // is active

}; // struct Paddle

// Represents block entity

struct Block
{
  Block();

  Vector    pos_;           // block global pos
  Vector    vel_;           // block velocity
  int       color_;         // block color
  bool      active_;        // is active

}; // struct Block

}  // namespace anshub

#endif  // GAME_BALL_N_PADDLE_H