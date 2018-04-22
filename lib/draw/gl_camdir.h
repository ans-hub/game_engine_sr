// *************************************************************
// File:    gl_camdir.h
// Descr:   holds camera direction settings
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef GC_GL_CAMDIR_H
#define GC_GL_CAMDIR_H

#include <limits>

namespace anshub {

// Stores direction settings

struct CamDir
{
  constexpr CamDir();
  constexpr CamDir(float reduce, float vel, float low, float high, bool locked);

  float reduce_;    // divident of direction angle
  float vel_;       // velocity 
  float low_;       // most low value of direction angle
  float high_;      // most high value of direction angle
  bool  locked_;    // is direction locked (can`t change)

}; // struct CamDir

inline constexpr CamDir::CamDir()
  : reduce_{1.0f}
  , vel_{1.0f}
  , low_{-std::numeric_limits<float>::min()}
  , high_{std::numeric_limits<float>::max()}
  , locked_{false}
{ }

inline constexpr CamDir::CamDir(
  float reduce, float vel, float low, float high, bool locked)
  : reduce_{reduce}
  , vel_{vel}
  , low_{std::fmod(low, 360.0f)}      // make in range
  , high_{std::fmod(high, 360.0f)}    //  -359.9... + 359.9... degrees
  , locked_{locked}
{ }

}  // namespace anshub

#endif  // GC_GL_CAMDIR_H