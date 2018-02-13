// *************************************************************
// File:    gl_camera.h
// Descr:   imitates camera for renderer (euler and uvn)
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

// Cameras conventions:
//  - all rotations in YXZ sequence
//  - dir_ is vector contains x (pitch), y (yaw), z (roll)

#ifndef GC_GL_CAMERA_H
#define GC_GL_CAMERA_H

#include "../math/trig.h"
#include "../math/vector.h"
#include "../math/matrix.h"

namespace anshub {

struct GlCamera
{
  GlCamera(
    float, float, int, int, const Vector&, const Vector&, float, float);

  TrigTable trig_;
  float   fov_;       // firld of view
  float   dov_;       // distance of view
  float   wov_;       // width of view plane
  float   z_near_;    // near z plane
  float   z_far_;     // far z plane
  int     scr_w_;     // screen width
  int     scr_h_;     // screen height
  float   ar_;        // aspect ratio
  Vector  vrp_;       // view reference point (world pos)
  
  // Euler specific

  Vector  dir_;       // cam direction angles (for Euler model, see note above)
  
  // UVN specific

  void    LookAt(const Vector&, float roll_hint = 0.0f);

  Vector  u_;         //
  Vector  v_;         // camera basis (as x,y,z) 
  Vector  n_;         // 
  Vector  target_;    // target point, look-at point
  
}; // struct GlCamera

}  // namespace anshub

#endif  // GC_GL_CAMERA_H

// Note : in educational purposes I don`t separate into different
// classes Euler and UVN cameras