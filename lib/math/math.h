// *************************************************************
// File:    math.h
// Descr:   math functions
// Author:  Novoselov Anton @ 2017-2018
// URL:     https://github.com/ans-hub/geomath_lib
// *************************************************************

#ifndef GM_MATH_H
#define GM_MATH_H

#include <cmath>
#include <memory>

#include "constants.h"

namespace anshub {

// Math helpers declaration

namespace math {

  int   Floor(float);
  int   Ceil(float);
  bool  Fzero(float);
  bool  FNotZero(float);
  bool  FlessZero(float);
  bool  Feq(float, float);
  bool  IsAbsFactorOfTwo(int);
  int   Clamp(int, int, int);
  float Clamp(float, float, float);
  template<class T = float>
  float LeadToRange(const std::pair<T,T>& src, const std::pair<T,T>& dst, T val);

} // namespace math

// Math helpers implementation

inline int math::Floor(float num)
{
  return (int)(num);
}

inline int math::Ceil(float num)
{
  return (int)(num + 1.0f);
}

inline bool math::Fzero(float num)
{
  return std::abs(num) < kEpsilon;
}

inline bool math::FNotZero(float num)
{
  return !(std::abs(num) < kEpsilon);
}

inline bool math::FlessZero(float num)
{
  return num < -kEpsilon;
}

inline bool math::Feq(float n1, float n2) 
{ 
  return std::abs(n1-n2) < kEpsilon;
}

inline bool math::IsAbsFactorOfTwo(int num)
{
  int n = std::abs(num);
  return !(n & (n-1));
}

inline int math::Clamp(int val, int min, int max)
{
  int rem = val % max;
  return rem < 0 ? max + rem : min + rem;
}

inline float math::Clamp(float val, float min, float max)
{
  float rem = std::fmod(val, max);
  return rem < 0.0f ? max + rem : min + rem;
}

// Make value from range 1 as value from range 2 

template<class T = float>
inline float math::LeadToRange(
  const std::pair<T,T>& range_src, const std::pair<T,T>& range_dest, T val)
{
  auto min_src = range_src.first;
  auto max_src = range_src.second;
  auto min_dest = range_dest.first;
  auto max_dest = range_dest.second;

  auto val_pos    = val / (max_src - min_src);
  auto val_offset = val_pos * (max_dest - min_dest);
  
  return min_dest + val_offset;
}

} // namespace anshub

#endif  // GM_MATH_H