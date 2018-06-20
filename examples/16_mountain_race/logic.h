// *************************************************************
// File:    logic.h
// Descr:   logic class for mountain race game
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef LOGIC_H
#define LOGIC_H

#include <utility>

#include "level.h"
#include "../helpers.h"

#include "lib/audio/audio_fx.h"
#include "lib/data/cfg_loader.h"
#include "lib/math/math.h"
#include "lib/window/gl_window.h"

namespace anshub {

struct Logic
{
  Logic(const Config&, GlWindow&, Level&);
  bool Process();

private:
  GlWindow& win_;
  Level& level_;
  Timer timer_;
  signed long ignore_input_;      // how much ms to ignore user input
  KbdBtn mem_forward_;            // memory for forward button
  KbdBtn mem_backward_;           // memory for backward button
  bool first_collision_;          // points that we collided first time

  void StartAudioEngineSound(const Config&);
  
  void ProcessSystemInput(Btn); 
  void ProcessTreesCollisions();
  void ProcessPlayerSounds();
  void ProcessPlayer();
  void ProcessIgnoreTime();

}; // struct Logic 

}  // namespace anshub

#endif // LOGIC_H