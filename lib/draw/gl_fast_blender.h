// *************************************************************
// File:    gl_fast_blender.h
// Descr:   struct that performs fast alpha color blending 
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef GL_FAST_BLENDER_H
#define GL_FAST_BLENDER_H

#include <iostream>
#include <functional>
#include <cmath>

#include "gl_aliases.h"
#include "fx_colors.h"
#include "exceptions.h"
#include "../math/math.h"

namespace anshub {

// Performs fast blending of colors with step 0.125f using only shifts
// and additions

struct FastBlender
{
  using FxBlend = std::function<void(Color<>&)>;

  FastBlender();

  void    SetAlphaFirst(float alpha);
  void    SetAlphaSecond(float alpha);
  float   GetAlphaFirst() const { return alpha_c1_; }
  float   GetAlphaSecond() const { return alpha_c2_; }
  FxBlend GetSecondBlender();
  
private:
  const float step_ = 0.125f;
  float alpha_c1_;
  float alpha_c2_;
  FxBlend fx_000_;
  FxBlend fx_125_;
  FxBlend fx_250_;
  FxBlend fx_375_;
  FxBlend fx_500_;
  FxBlend fx_625_;
  FxBlend fx_750_;
  FxBlend fx_875_;
  FxBlend fx_1000_;

}; // struct FastBlender

namespace fast_blender {

  void fx_000(Color<>&);
  void fx_125(Color<>&);
  void fx_250(Color<>&);
  void fx_375(Color<>&);
  void fx_500(Color<>&);
  void fx_625(Color<>&);
  void fx_750(Color<>&);
  void fx_875(Color<>&);
  void fx_1000(Color<>&);

} // namespace fast_blender

// Constructs FastBlender and set alpha_c1

inline FastBlender::FastBlender()
  : alpha_c1_ {}
  , alpha_c2_ {}
  , fx_000_ {&fast_blender::fx_000}
  , fx_125_ {&fast_blender::fx_125}
  , fx_250_ {&fast_blender::fx_250}
  , fx_375_ {&fast_blender::fx_375}
  , fx_500_ {&fast_blender::fx_500}
  , fx_625_ {&fast_blender::fx_625}
  , fx_750_ {&fast_blender::fx_750}
  , fx_875_ {&fast_blender::fx_875}
  , fx_1000_{&fast_blender::fx_1000}
{ }

inline void FastBlender::SetAlphaFirst(float alpha)
{
  alpha_c1_ = std::round(alpha / step_) * step_;
  alpha_c2_ = 1.0f - alpha_c1_;
}

// Return blending function in depends of alpha_c1_

inline FastBlender::FxBlend FastBlender::GetSecondBlender()
{
  if (alpha_c2_ < 0.125f)
    return fx_000_; 
  else if (alpha_c2_ == 0.125f)
    return fx_125_;
  else if (alpha_c2_ == 0.250f)
    return fx_250_;
  else if (alpha_c2_ == 0.375f)
    return fx_375_;
  else if (alpha_c2_ == 0.500f)
    return fx_500_;
  else if (alpha_c2_ == 0.625f)
    return fx_625_;
  else if (alpha_c2_ == 0.750)
    return fx_750_;
  else if (alpha_c2_ == 0.875f)
    return fx_875_;
  else
    return fx_1000_;
}

// Implementation of helper functions

inline void fast_blender::fx_000 (Color<>& c)
{ 
  c.r_ = 0; c.g_ = 0; c.b_ = 0;
}

inline void fast_blender::fx_125(Color<>& c)
{ 
  c.r_ >>= 3; c.g_ >>= 3; c.b_ >>= 3;
}

inline void fast_blender::fx_250(Color<>& c)
{ 
  c.r_ >>= 2; c.g_ >>= 2; c.b_ >>= 2;
}

inline void fast_blender::fx_375(Color<>& c)
{
  c.r_ = (c.r_ >> 2) + (c.r_ >> 3);
  c.g_ = (c.g_ >> 2) + (c.g_ >> 3);
  c.b_ = (c.b_ >> 2) + (c.b_ >> 3);
}

inline void fast_blender::fx_500(Color<>& c)
{
  c.r_ >>= 1; c.g_ >>= 1; c.b_ >>= 1;
}

inline void fast_blender::fx_625(Color<>& c)
{
  c.r_ = (c.r_ >> 1) + (c.r_ >> 3);
  c.g_ = (c.g_ >> 1) + (c.g_ >> 3);
  c.b_ = (c.b_ >> 1) + (c.b_ >> 3);
}

inline void fast_blender::fx_750(Color<>& c)
{
  c.r_ = (c.r_ >> 1) + (c.r_ >> 2);
  c.g_ = (c.g_ >> 1) + (c.g_ >> 2);
  c.b_ = (c.b_ >> 1) + (c.b_ >> 2);
}

inline void fast_blender::fx_875(Color<>& c)
{
  c.r_ = (c.r_ >> 1) + (c.r_ >> 2) + (c.r_ >> 3);
  c.g_ = (c.g_ >> 1) + (c.r_ >> 2) + (c.g_ >> 3);
  c.b_ = (c.b_ >> 1) + (c.r_ >> 2) + (c.b_ >> 3);
}

inline void fast_blender::fx_1000(Color<>&) {}

}  // namespace anshub

#endif  // GL_FAST_BLENDER_H