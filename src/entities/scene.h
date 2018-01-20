// *************************************************************
// File:    scene.h
// Descr:   builds visual game scene
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef GAME_SCENE_H
#define GAME_SCENE_H

#include <sstream>
#include <iomanip>
#include <cmath>

#include "lib/window/gl_window.h"
#include "lib/system/rand_toolkit.h"
#include "lib/system/timer.h"
#include "lib/draw/gl_buffer.h"
#include "lib/draw/gl_draw.h"
#include "lib/draw/gl_text.h"
#include "lib/draw/fx_colors.h"
#include "lib/math/fx_polygons.h"

#include "src/entities/level.h"

namespace anshub {

class Scene
{
public:
  Scene(GlWindow&, Level&);
  void Build();

private:
  using slong = signed long;

  void DrawStarfield();
  void DrawWarships();
  void DrawWarshipsAttack();
  void DrawShake();
  void DrawCannon();
  void DrawExplosions();

  void PrintInfo();
  void CountFPS();

  // Helpers data members for fast access
  
  GlWindow& win_;
  int       w_;       // current scene width (eq window width)
  int       h_;       // current scene height (eq window height)
  int       half_w_;  // half width for fast access
  int       half_h_;  // half height for fast access

  // Main data members

  Level&    level_;   
  Timer     timer_;
  Buffer    buffer_;
  GlText    text_;
  
  // Data members to store info

  int       curr_fps_;
  int       prev_fps_;
  slong     time_passed_;

}; // class Scene

}  // namespace anshub

#endif  // GAME_SCENE_H