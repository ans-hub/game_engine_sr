// *************************************************************
// File:    logic.h
// Descr:   game logic
// Author:  Novoselov Anton @ 2017
// *************************************************************

#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include "lib/window/gl_window.h"
#include "lib/audio/audio_out.h"
#include "lib/system/rand_toolkit.h"
#include "lib/render/fx_colors.h"
#include "lib/math/vector.h"
#include "lib/math/polygon.h"
#include "lib/system/timer.h" 

#include "config.h"
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