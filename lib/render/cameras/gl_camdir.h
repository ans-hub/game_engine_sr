// *************************************************************
// File:    gl_camdir.h
// Descr:   holds camera direction settings
// Author:  Novoselov Anton @ 2017
// *************************************************************

#ifndef GC_GL_CAMDIR_H
#define GC_GL_CAMDIR_H

#include <limits>

namespace anshub {

//***************************************************************************
// Holds settings for camera and object directions (see note after code) 
//***************************************************************************

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

//***************************************************************************
// Inline implementation 
//***************************************************************************

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

// Note:
//  - "reduce" used to reduce the amount of angle, i.e. curr yaw rotation
//    to the "theta of yaw" eq 60 with "reduce" eq 2.0f will be 30 degrees
//  - "vel" is velocity of direction changes on one frame
//  - "low" and "high" - the maximum angles of directions. When 0.0f and 0.0f,
//    then we no limits
//  - locked - is rotation of direction is locked 