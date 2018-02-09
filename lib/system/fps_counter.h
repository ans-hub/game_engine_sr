// *************************************************************
// File:    fps_counter.h
// Descr:   simple fps counter
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef SYS_FPS_COUNTER_H
#define SYS_FPS_COUNTER_H

#include "timer.h"

namespace anshub {

struct FpsCounter
{
  FpsCounter() : timer_{}, curr_{}, prev_{}, time_passed_{} { } 
  void Count();
  long ReadPrev() const { return prev_; }

private:
  Timer timer_;
  int   curr_;
  int   prev_;
  long  time_passed_;
  
}; // struct FpsCounter

}  // namespace anshub

#endif  // SYS_FPS_COUNTER_H