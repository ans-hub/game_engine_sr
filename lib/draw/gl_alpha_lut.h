// *************************************************************
// File:    gl_alpha_lut.h
// Descr:   alpha blending lookup table
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef GL_ALPHA_LUT_H
#define GL_ALPHA_LUT_H

#include <iostream>
#include <vector>
#include <cmath>

#include "exceptions.h"

namespace anshub {

// Represent look up table for fast color alpha blending. As we know,
// Color_Total = (Color_1 (1-&)) + (Color_2 * &). To speed up calculations
// inside rasterizer we precalculate this value for one give alpha value.

// In the start of programm we create lookup table for some values. Inside
// renderer loop every triangle is checked for alpha channel. In depends of
// this channel we choose particular alpha lut table and recieve it inside
// renderer

struct AlphaLut
{
  AlphaLut();
  explicit AlphaLut(int levels);
  int operator()(float alpha, int c1, int c2) const;

private:
  std::vector<std::vector<int>> data_;

}; // struct AlphaLut

// Creates default look up table with 3 levels of alpha blending

inline AlphaLut::AlphaLut() : AlphaLut(3) { }

// Creates Alpha look up table by given levels. Max possible levels are 9

inline AlphaLut::AlphaLut(int levels)
{
  if (levels > 255)
    throw DrawExcept("Max possible levels of alpha is 255");
  
  // Prepare interpolants

  float dx_alpha = 1.0f / (levels + 1);
  float alpha_1 = 0.0f;
  float alpha_2 = 1.0f;
  
  // Fill subarray for each alpha blending level, where size of subarray
  // is 256*256 (all possible colors after alpha blending)

  data_.reserve(levels);
  for (auto& level : data_)
  {
    alpha_1 += dx_alpha;
    alpha_2 -= dx_alpha;
    level.reserve(256*256);
    
    for (int c1 = 0; c1 < 256; ++c1)
      for (int c2 = 0; c2 < 256; ++c2)
        level[c1*256+c2] = c1*alpha_1 - c2*alpha_2;
  }
}

// Returns blended color for given alpha

inline int AlphaLut::operator()(float alpha, int c1, int c2) const
{
  int alpha_index = alpha * (data_.size() + 2) - 1.0f;
  if (alpha_index < 0)
    return c1;
  else if (alpha_index >= data_.size())
    return c2;
  else
    return data_[alpha_index][c1*256+c2];
}

}  // namespace anshub

#endif  // GL_ALPHA_LUT_H