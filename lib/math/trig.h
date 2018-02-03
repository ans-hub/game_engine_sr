// ***************************************************************
// File:    trig.h
// Descr:   precalculated trigonometry table and helper functions
// Author:  Novoselov Anton @ 2017-2018
// URL:     https://github.com/ans-hub/geomath_lib
// ***************************************************************

#ifndef GM_TRIG_H
#define GM_TRIG_H

#include <vector>
#include <cmath>

#include "constants.h"

namespace anshub {

class TrigTable
{
public:
  TrigTable();
  float Sin(float) const;
  float Cos(float) const;
private:
  std::vector<float> sine_;
  std::vector<float> cosine_;

}; // class TrigTable

namespace trig {

  float Rad2deg(float);
  float Deg2rad(float);
  float CalcOppositeCatet(float adjanced_catet, float theta, TrigTable&);

} // namespace math

// Member functions implementation

inline TrigTable::TrigTable()
  : sine_(361)
  , cosine_(361)
{
  for (int i = 0; i < 361; ++i) 
  {
    sine_[i]    = std::sin(trig::Deg2rad(i));
    cosine_[i]  = std::cos(trig::Deg2rad(i));
  }
  
  // Exclude negative zeroes

  sine_[180] = 0;
  sine_[360] = 0;
  cosine_[90] = 0;
  cosine_[270] = 0;
}

inline float TrigTable::Sin(float theta) const
{
  theta = fmodf(theta, 360);
  if (theta < 0)
    theta += 360.0f;
  int t_int = static_cast<int>(theta);
  float t_frac = theta - t_int;
  return (
    sine_[t_int] + t_frac * (sine_[t_int+1] - sine_[t_int])
  );
}

inline float TrigTable::Cos(float theta) const
{
  theta = fmodf(theta, 360);
  if (theta < 0)
    theta += 360.0f;
  int t_int = static_cast<int>(theta);
  float t_frac = theta - t_int;
  return (
    cosine_[t_int] + t_frac * (cosine_[t_int+1] - cosine_[t_int])
  );
}

// Trigonometry helpers implementation

inline float trig::Deg2rad(float deg)
{ 
  return deg * kPI / 180.0;
}

inline float trig::Rad2deg(float rad)
{
  return rad * 180.0 / kPI;
}

// cos& = adj / hyp
// hyp  = adj / cos&

inline float trig::CalcOppositeCatet(
  float adjanced_catet, float theta, TrigTable& tab)
{
  float cosine = tab.Cos(theta);
  float hypotenuse = adjanced_catet / cosine;
  float opposite_catet = tab.Sin(theta) * hypotenuse;
  return opposite_catet;
}

} // namespace anshub

#endif  // GM_TRIG_H