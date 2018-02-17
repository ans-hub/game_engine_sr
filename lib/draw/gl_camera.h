// *************************************************************
// File:    gl_camera.h
// Descr:   imitates camera for renderer (euler and uvn)
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

// Cameras conventions:
//  - all rotations in YXZ sequence
//  - dir_ is vector contains Euler`s x (pitch), y (yaw), z (roll)
//  - view_ direction vector

#ifndef GC_GL_CAMERA_H
#define GC_GL_CAMERA_H

#include "../math/trig.h"
#include "../math/vector.h"
#include "../math/matrix.h"
#include "../math/matrix_rotate_eul.h"

namespace anshub {

struct GlCamera
{

  // HELPER STRUCTS

  struct Gimbal
  {
    Gimbal()
    : steps_{1}, target_{}, velocity_{}, speed_{1} { }
    int     steps_;
    Vector  target_;
    Vector  velocity_;
    int     speed_;

  }; // struct Gimbal

  enum class Type {
    EULER,
    UVN
  };

  // MEMBER DEFINITIONS

  GlCamera(
    float, float, int, int, const Vector&, const Vector&, float, float);

  void  SwitchType(Type);
  void  ChangeFov(int);
  void  RefreshViewVector();
  
  TrigTable trig_;
  Type      type_;      // camera type    
  float     fov_;       // firld of view
  float     dov_;       // distance of view
  float     wov_;       // width of view plane
  float     z_near_;    // near z plane
  float     z_far_;     // far z plane
  int       scr_w_;     // screen width
  int       scr_h_;     // screen height
  float     ar_;        // aspect ratio
  Vector    vrp_;       // view reference point (world pos)
  
  // Euler specific

  Vector  dir_;       // cam direction angles (for Euler model)
  Vector  view_;       // cam view vector (corresponds to dir_ vector)
  
  // UVN specific

  void    LookAt(const Vector&, float roll_hint = 0.0f);

  Vector  u_;           //
  Vector  v_;           // camera basis (as x,y,z) 
  Vector  n_;           // 
  Gimbal  gimbal_;      // target point, look-at point

}; // struct GlCamera

//***************************************************************************
// HELPERS DECLARATION
//***************************************************************************

namespace camera {

  constexpr int kDefaultGimbalSpeed = 15;
  constexpr int kMinFov = 45;
  constexpr int kMaxFov = 120;

}  // namespace camera

//***************************************************************************
// INLINE IMPLEMENTATION
//***************************************************************************

inline void GlCamera::ChangeFov(int fov)
{ 
  fov_ = std::min(camera::kMaxFov, std::max(camera::kMinFov, fov));
  wov_ = 2 * trig::CalcOppositeCatet(dov_, fov_/2, trig_);
}

}  // namespace anshub

#endif  // GC_GL_CAMERA_H

// Note : in educational purposes I don`t separate into different
// classes Euler and UVN cameras