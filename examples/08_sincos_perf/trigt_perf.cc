// *************************************************************
// File:    sincos_perf.cc
// Descr:   test sin_cos table perfomance in comparsion with cmath
// Author:  Novoselov Anton @ 2017
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#include <iostream>

#include "lib/system/rand_toolkit.h"
#include "lib/math/trig.h"

int main()
{ 
  rand_toolkit::start_rand();

  constexpr int cnt {100000000};
  float prod {0};
  anshub::TrigTable trig {};

  for (int i = 0; i < cnt; ++i)
  { 
    float angle = rand_toolkit::get_rand(0.0f, 359.0f);
    prod += trig.Sin(angle);
    prod += trig.Cos(angle);
  }
  std::cerr << prod << '\n';
  return 0;
}