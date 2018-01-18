// *************************************************************
// File:    timer.h
// Descr:   timer class
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef SYS_TIMER_H
#define SYS_TIMER_H

#include <iostream>
#include <chrono>

namespace anshub {

class Timer
{
public:
  using slong = signed long;
  
  Timer();
  explicit Timer(int frames);

  void Start();
  void Wait();
  void End();
  
  inline slong GetStartTime() const { return start_time_; }
  inline slong GetEndTime() const { return end_time_; }
  slong GetCurrentClock() const;
private:
  int   ms_wait_;
  slong start_time_;
  slong end_time_;

}; // class Timer 

}  // namespace anshub

#endif  // SYS_TIMER_H