// *************************************************************
// File:    math.h
// Descr:   math functions
// Author:  Novoselov Anton @ 2017-2018
// URL:     https://github.com/ans-hub/geomath_lib
// *************************************************************

#ifndef GM_MATH_H
#define GM_MATH_H

#include <vector>
#include <cmath>

#include "constants.h"

namespace anshub {

namespace math {

  using Table = std::vector<double>;

  // Float helpers

  bool    Fzero(double);
  bool    FlessZero(double);
  bool    Feq(double, double);

  // Trigonometry helpers

  double  Rad2deg(double);
  double  Deg2rad(double);
  Table   BuildSinTable();
  Table   BuildCosTable();
  double  FastSinCos(const Table&, double);

} // namespace math

} // namespace anshub

#endif  // GM_MATH_H