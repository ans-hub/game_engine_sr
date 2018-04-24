// ***************************************************************
// File:    trig.h
// Descr:   precalculated trigonometry table and helper functions
// Author:  Novoselov Anton @ 2017-2018
// URL:     https://github.com/ans-hub/geomath_lib
// ***************************************************************

#ifndef GM_TRIG_H
#define GM_TRIG_H

#include <array>
#include <cmath>

#include "constants.h"

namespace anshub {

//****************************************************************************
// Precomputed LUT contains sin/cos angle values
//****************************************************************************

struct TrigTable
{
  constexpr TrigTable();
  constexpr float Sin(float) const;
  constexpr float Cos(float) const;

private:
  std::array<float, 361> sine_;
  std::array<float, 361> cosine_;

}; // class TrigTable

//****************************************************************************
// Helper functions
//****************************************************************************

namespace trig {

  constexpr float Rad2deg(float);
  constexpr float Deg2rad(float);
  float CalcOppositeCatet(float adjanced_catet, float theta, const TrigTable&);
  float CalcOppositeCatet(float adjanced_catet, float theta);

} // namespace math

//****************************************************************************
// Inline member functions implementation
//****************************************************************************

inline constexpr TrigTable::TrigTable()
  : sine_{}
  , cosine_{}
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

inline constexpr float TrigTable::Sin(float theta) const
{
  if (theta > 360.0f || theta < -360.0f)
    theta = fmodf(theta, 360.0f);
  if (theta < 0.0f)
    theta += 360.0f;
  int t_int = static_cast<int>(theta);
  float t_frac = theta - t_int;
  return (
    sine_[t_int] + t_frac * (sine_[t_int+1] - sine_[t_int])
  );
}

inline constexpr float TrigTable::Cos(float theta) const
{
  if (theta > 360.0f || theta < -360.0f)
    theta = fmodf(theta, 360.0f);
  if (theta < 0.0f)
    theta += 360.0f;
  int t_int = static_cast<int>(theta);
  float t_frac = theta - t_int;
  return (
    cosine_[t_int] + t_frac * (cosine_[t_int+1] - cosine_[t_int])
  );
}

//****************************************************************************
// Inline trigonometry helpers implementation
//****************************************************************************

inline constexpr float trig::Deg2rad(float deg)
{ 
  return deg * math::kPI / 180.0;
}

inline constexpr float trig::Rad2deg(float rad)
{
  return rad * 180.0 / math::kPI;
}

// Computes opposite catet in triangle with right angle:
//  cos_& = adj / hyp
//  hyp   = adj / cos_&
//  opp   = sin_& * hyp
// Also we may compute oppisite catet from tg:
//  tg_& = sin_& / cos_&
//  tg_& = opp / adj
//  opp = tg_& * adj = (sin_& / cos_&) * adj 

inline float trig::CalcOppositeCatet(
  float adjanced_catet, float theta, const TrigTable& trig)
{
  float cosine = trig.Cos(theta);
  float hypotenuse = adjanced_catet / cosine;
  float opposite_catet = trig.Sin(theta) * hypotenuse;
  return opposite_catet;
}

// The same as above but using std trigonometry functions

inline float trig::CalcOppositeCatet(float adjanced_catet, float theta)
{
  float cosine = std::cos(theta);
  float hypotenuse = adjanced_catet / cosine;
  float opposite_catet = std::sin(theta) * hypotenuse;
  return opposite_catet;
}

} // namespace anshub

#endif  // GM_TRIG_H