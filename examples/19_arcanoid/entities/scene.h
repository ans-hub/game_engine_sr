// *************************************************************
// File:    scene.h
// Descr:   simple 3d processor (just visualiseы all objects)
// Author:  Novoselov Anton @ 2018
// *************************************************************

#ifndef GAME_SCENE_H
#define GAME_SCENE_H

#include <sstream>
#include <iomanip>
#include <cmath>

#include "lib/window/gl_window.h"
#include "lib/system/rand_toolkit.h"
#include "lib/system/timer.h"
#include "lib/render/gl_scr_buffer.h"
#include "lib/render/gl_draw.h"
#include "lib/render/gl_text.h"
#include "lib/render/fx_colors.h"
#include "lib/math/polygon.h"
#include "lib/math/segment.h"
#include "lib/math/math.h"
#include "lib/math/trig.h"

#include "entities/level.h"

namespace anshub {

class Scene
{
public:
  Scene(GlWindow&, Level&);
  void Build();

private:
  using slong = signed long;

  void DrawPaddle();
  void DrawBlocks();
  void DrawBalls();

  void DrawRectangle(const Vector& pos, float x_offset, float y_offset, int color);

  void PrintInfo();
  void PrintCentered(const char*);
  void CountFPS();

  GlWindow& win_;
  int       w_;       // current scene width (eq window width)
  int       h_;       // current scene height (eq window height)
  int       half_w_;  // half width for fast access
  int       half_h_;  // half height for fast access

  Level&    level_;   
  Timer     timer_;
  ScrBuffer buffer_;
  GlText    text_;
  
  int         curr_fps_;
  int         prev_fps_;
  slong       time_passed_;
  TrigTable   trig_table_;

}; // class Scene

}  // namespace anshub

#endif  // GAME_SCENE_H