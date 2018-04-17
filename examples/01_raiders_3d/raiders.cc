// *************************************************************
// File:    raiders.cc
// Descr:   main game loop
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#include "lib/window/gl_window.h"
#include "lib/window/helpers.h"
#include "lib/audio/audio_out.h"
#include "lib/system/rand_toolkit.h"
#include "lib/system/timer.h"

#include "entities/level.h"
#include "entities/logic.h"
#include "entities/scene.h"

using namespace anshub;

int main()
{
  // System settings

  constexpr int kWidth = 800;
  constexpr int kHeight = 600;
  constexpr int kFpsWait = 35;
  constexpr int kShipsCnt = 13;

  // Create window and usefull stuff
 
  auto pos  = io_helpers::GetXYToMiddle(kWidth, kHeight); 
  auto mode = io_helpers::FindVideoMode(kWidth, kHeight);

  GlWindow win (pos.x, pos.y, kWidth, kHeight, "Raiders 3D");
  win.ToggleFullscreen(mode);
  win.HideCursor();
  rand_toolkit::start_rand();

  // Create game entities

  Timer     timer   (kFpsWait);
  AudioOut  audio   {};
  Level     level   (kShipsCnt);
  Logic     logic   {win, level, audio};
  Scene     scene   {win, level};

  // Main loop

  do {
    timer.Start();
    if (logic.Process()) {
      win.Clear();
      scene.Build();
      win.Render();
      timer.Wait();
    }
    else
      win.Close();
  } while (!win.Closed());

  return 0;
}

// *************************************************************
// Todo:
// - volume inc/dec in audio_out
// - fix rand toolkit get_rand
// - ud-lr shake
// - broken enemy
// - add warships blur in depend of velocity
// - config class
// - white splash in enemy ship when its shot
// - custom rotate ships
// - add alternative - broken ship
// - delta time in main loop (???)
// - add friend ships left top and right top
// - slow motion when ship is near and broken
// *************************************************************
