// *************************************************************
// File:    logic.h
// Descr:   main game logic
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include "lib/window/gl_window.h"
#include "lib/audio/audio_out.h"
#include "lib/system/rand_toolkit.h"
#include "lib/math/fx_colors.h"
#include "lib/system/timer.h" 

#include "src/config.h"
#include "src/entities/enums.h"
#include "src/entities/aliases.h"
#include "src/entities/level.h"

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

  void ProcessExplosions();
  void PrepareExplosion(Starship&);
  void ProcessCannon(Pos&, Btn);
  void ProcessVelocity(Btn);
  bool ProcessGameState(Btn);
  
  GlWindow&   win_;
  Level&      level_;
  AudioOut&   audio_;
  GameState   state_;

}; // class Logic

}  // namespace anshub

#endif  // GAME_LOGIC_H