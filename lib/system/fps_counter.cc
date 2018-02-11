// *************************************************************
// File:    fps_counter.cc
// Descr:   simple fps counter
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#include "fps_counter.h"

namespace anshub {

void FpsCounter::Count()
{
  timer_.End();
  ++curr_;
  time_passed_ += timer_.GetEndTime() - timer_.GetStartTime();
  if (time_passed_ >= 1000) {
    time_passed_ = 0;
    prev_ = curr_;
    curr_ = 0;
  }
  timer_.Start(); 
}

}  // namespace anshub