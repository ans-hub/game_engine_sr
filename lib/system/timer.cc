// *************************************************************
// File:    timer.cc
// Descr:   timer class
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#include "timer.h"

namespace anshub {

Timer::Timer()
  : ms_wait_{}
  , start_time_{}
  , end_time_{}
{ }

Timer::Timer(int frames)
  : ms_wait_{1000/frames}
  , start_time_{}
  , end_time_{}
{ }

void Timer::Wait()
{
  timespec ts;
  ts.tv_sec  = ms_wait_ / 1000;
  ts.tv_nsec = ms_wait_ * 1000000;

  while ((nanosleep(&ts, &ts) == -1) && (errno == EINTR)) { }
  end_time_ = GetCurrentClock();
}
 
} // namespace anshub

// Note : remainder - second = 1/1e3 milli, 1/e6 - micro, 1/1e9 nano