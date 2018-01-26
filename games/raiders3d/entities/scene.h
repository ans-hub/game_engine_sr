// *************************************************************
// File:    scene.h
// Descr:   simple 3d processor (just visualise all objects)
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
#include "lib/math/polygon.h"
#include "lib/math/segment.h"
#include "lib/math/math.h"

#include "entities/level.h"

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
  void DrawCannon();
  void DrawExplosions();

  void PrintInfo();
  void PrintCentered(const char*);
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
  
  // Math tables

  math::Table sin_tab_;
  math::Table cos_tab_;

}; // class Scene

}  // namespace anshub

#endif  // GAME_SCENE_H