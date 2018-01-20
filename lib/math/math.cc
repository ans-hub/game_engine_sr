// *************************************************************
// File:    math.cc
// Descr:   math functions
// Author:  Novoselov Anton @ 2017
// URL:     https://github.com/ans-hub/geomath_lib
// *************************************************************

#include "math.h"

namespace anshub {

double math::Deg2rad(double deg)
{ 
  return deg * kPI / 180.0;
}

double math::Rad2deg(double rad)
{
  return rad * 180.0 / kPI;
}

bool math::Fzero(double num)
{ 
  return std::abs(num) < kEpsilon;
}

bool math::Feq(double n1, double n2) 
{ 
  return std::abs(n1-n2) < kEpsilon;
}

math::Table math::BuildSinTable()
{
  math::Table t(361);
  for (int i = 0; i < 361; ++i)
    t[i] = std::sin(Deg2rad(i));
  return t;
}

math::Table math::BuildCosTable()
{
  math::Table t(361);
  for (int i = 0; i < 361; ++i)
    t[i] = std::cos(Deg2rad(i));
  return t;
}

double math::FastSinCos(const math::Table& t, double theta)
{
  theta = fmodf(theta,360);
  if (theta < 0) theta += 360;
  int t_int = static_cast<int>(theta);
  double t_frac = theta - t_int;
  return ( t[t_int] + t_frac * (t[t_int+1] - t[t_int]) );
}

} // namespace anshub