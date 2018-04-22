// *************************************************************
// File:    gl_camera.h
// Descr:   camera based on Euler`s angles
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

// Notes:
// - in draw lib all transformations is based on camera direction which
//   used Euler`s angles. Thus we use Euler`s camera as base camera
// - all rotations in YXZ sequence
// - dir_ is vector contains Euler`s x (pitch), y (yaw), z (roll)
// - view_ is direction vector of camera
// - view_ vector is computes in reverse order (ZXY)

#ifndef GC_GL_CAMERA_H
#define GC_GL_CAMERA_H

#include <cmath>

#include "lib/draw/gl_aliases.h"
#include "lib/draw/gl_enums.h"
#include "lib/draw/gl_camdir.h"

#include "lib/math/trig.h"
#include "lib/math/vector.h"
#include "lib/math/matrix.h"
#include "lib/math/matrix_rotate_eul.h"

namespace anshub {

//***************************************************************************
// EULER CAMERA INTERFACE
//***************************************************************************

struct GlCamera
{
  using CamTypes = CamType::Types;
  enum DirectionType { YAW, PITCH, ROLL };

  GlCamera(float fov, float dov, int scr_w, int scr_h,
    cVector& vrp, cVector& dir, float z_near, float z_far, cTrigTable&
  );
  GlCamera(const GlCamera&) =default;
  GlCamera& operator=(const GlCamera&) =default;
  GlCamera(GlCamera&&) =default;
  GlCamera& operator=(GlCamera&&) =default;
  virtual ~GlCamera() { }
  virtual void Preprocess() { }

  void ChangeFov(int new_fov);
  
  template<class ... Args>
  void SetDirection(DirectionType, Args&&...);

  // General camera`s settings

  float   fov_;       // firld of view
  float   dov_;       // distance of view
  float   wov_;       // width of view plane
  float   z_near_;    // near z plane
  float   z_far_;     // far z plane
  int     scr_w_;     // screen width
  int     scr_h_;     // screen height
  float   ar_;        // aspect ratio
  Vector  vrp_;       // view reference point
  Vector  dir_;       // Euler`s direction angles
  
  CamTypes    type_;  // camera type
  cTrigTable& trig_;

  CamDir  pitch_;
  CamDir  yaw_;
  CamDir  roll_;

}; // struct GlCamera

//***************************************************************************
// CAMERA HELPERS
//***************************************************************************

namespace camera_helpers {

  Vector ComputeCamViewVector(cVector& cam_direction, cTrigTable&);

} // namespace camera_helpers

//***************************************************************************
// CAMERA CONSTANTS
//***************************************************************************

namespace camera_const {

  constexpr int kMinFov = 45;
  constexpr int kMaxFov = 120;

} // namespace camera_const

//***************************************************************************
// INLINE IMPLEMENTATION
//***************************************************************************

inline void GlCamera::ChangeFov(int fov)
{ 
  fov_ = std::min(camera_const::kMaxFov, std::max(camera_const::kMinFov, fov));
  wov_ = 2 * trig::CalcOppositeCatet(dov_, fov_/2, trig_);
}
    
template<class ... Args>
inline void GlCamera::SetDirection(DirectionType type, Args&& ...args)
{
  switch(type)
  {
    case GlCamera::DirectionType::YAW :
      yaw_ = CamDir(std::forward<Args>(args)...);
      break;
    case GlCamera::DirectionType::PITCH:
      pitch_ = CamDir(std::forward<Args>(args)...);
      break;
    case GlCamera::DirectionType::ROLL :
      roll_ = CamDir(std::forward<Args>(args)...);
      break;
    default: break;
  }
}

} // namespace anshub

#endif  // GC_GL_CAMERA_H