// *************************************************************
// File:    arkanoid.cc
// Descr:   main game loop
// Author:  Novoselov Anton @ 2018
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
  constexpr int kWidth = 800;
  constexpr int kHeight = 600;
  constexpr int kFpsWait = 35;
  constexpr int kBallsCnt = 3;

  auto pos  = io_helpers::GetXYToMiddle(kWidth, kHeight); 
  auto mode = io_helpers::FindVideoMode(kWidth, kHeight);

  GlWindow win (pos.x, pos.y, kWidth, kHeight, "Arkanoid");
  win.HideCursor();
  rand_toolkit::start_rand();

  Timer     timer   (kFpsWait);
  AudioOut  audio   {};
  Level     level   (kBallsCnt);
  Logic     logic   {win, level, audio};
  Scene     scene   {win, level};

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