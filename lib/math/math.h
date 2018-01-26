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
  
  double Deg2rad(double);
  double Rad2deg(double);
  bool Fzero(double);
  bool FlessZero(double);
  bool Feq(double, double);

  Table BuildSinTable();
  Table BuildCosTable();

  double FastSinCos(const Table&, double);

} // namespace math

} // namespace anshub

#endif  // GM_MATH_H