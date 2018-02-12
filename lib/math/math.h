// *************************************************************
// File:    math.h
// Descr:   math functions
// Author:  Novoselov Anton @ 2017-2018
// URL:     https://github.com/ans-hub/geomath_lib
// *************************************************************

#ifndef GM_MATH_H
#define GM_MATH_H

#include <cmath>

#include "constants.h"

namespace anshub {

// Math helpers declaration

namespace math {

  bool  Fzero(float);
  bool  FNotZero(float);
  bool  FlessZero(float);
  bool  Feq(float, float);
  float Clamp(float, float, float);

} // namespace math

// Math helpers implementation

inline bool math::Fzero(float num)
{
  return std::abs(num) < kEpsilon;
}

inline bool math::FNotZero(float num)
{
  return std::abs(num) < kEpsilon;
}

inline bool math::FlessZero(float num)
{
  return num < kEpsilon;
}

inline bool math::Feq(float n1, float n2) 
{ 
  return std::abs(n1-n2) < kEpsilon;
}

inline float math::Clamp(float d, float min, float max)
{
  const float t = d < min ? min : d;
  return t > max ? max : t;
}

} // namespace anshub

#endif  // GM_MATH_H