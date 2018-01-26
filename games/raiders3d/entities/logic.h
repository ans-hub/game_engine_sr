// *************************************************************
// File:    logic.h
// Descr:   simple logic processor (manipilates game objects)
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

// Note: real logic processor shouldn't include this modules: physics,
// ai, animation and collision detection. They should be separated.
// Logic processor should only manage all game objects and game logic

#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include "lib/window/gl_window.h"
#include "lib/audio/audio_out.h"
#include "lib/system/rand_toolkit.h"
#include "lib/draw/fx_colors.h"
#include "lib/math/point.h"
#include "lib/math/fx_polygons.h"
#include "lib/system/timer.h" 

#include "config.h"
#include "entities/aliases.h"
#include "entities/level.h"

namespace anshub {

class Logic
{
public:
  Logic(GlWindow&, Level&, AudioOut&);
  bool Process();

private:
  void InitCannon();
  void InitStarfield();
  void InitWarship(Starship&);

  void MoveStarfield();
  void MoveWarships();

  void PrepareEnemyAttack();
  void PrepareExplosion(Starship&);

  void ProcessEnemyAttack();
  void ProcessPlayerShots();
  void ProcessExplosions();
  void ProcessCannon(Pos&, Btn);
  void ProcessViewport(Btn);
  bool ProcessGameState(Btn);
  
  GlWindow&   win_;
  Level&      level_;
  AudioOut&   audio_;

}; // class Logic

}  // namespace anshub

#endif  // GAME_LOGIC_H