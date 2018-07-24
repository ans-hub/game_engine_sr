// *************************************************************
// File:    logic.h
// Descr:   simple logic processor (manipilates game objects)
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include "lib/window/gl_window.h"
#include "lib/audio/audio_out.h"
#include "lib/system/rand_toolkit.h"
#include "lib/draw/fx_colors.h"
#include "lib/math/vector.h"
#include "lib/math/polygon.h"
#include "lib/system/timer.h" 

#include "config.h"
#include "entities/level.h"
#include "entities/game_objects.h"

namespace anshub {

class Logic
{
public:
  Logic(GlWindow&, Level&, AudioOut&);
  bool Process();

private:

  void InitLogic();
  void InitPaddle();
  void InitBalls();
  void InitBlocks();
  void ProcessBalls(Btn);
  void MovePaddle(Btn);
  bool ProcessGameState(Btn);
  
  GlWindow&   win_;
  Level&      level_;
  AudioOut&   audio_;

}; // class Logic

}  // namespace anshub

#endif  // GAME_LOGIC_H