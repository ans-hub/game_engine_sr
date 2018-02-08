// *************************************************************
// File:    gl_camera_euler.h
// Descr:   imitates euler camera for renderer
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef GC_GL_CAMERA_EULER_H
#define GC_GL_CAMERA_EULER_H

#include "../math/vector.h"
#include "../math/matrix.h"

namespace anshub {

struct GlCameraEuler
{
  GlCameraEuler(float, float, int, int, const Vector&, const Vector&, float, float);

  TrigTable trig_;
  float     fov_;       // firld of view
  float     dov_;       // distance of view
  float     wov_;       // width of view plane
  float     z_near_;    // near z plane
  float     z_far_;     // far z plane
  int       scr_w_;     // screen width
  int       scr_h_;     // screen height
  float     ar_;        // aspect ratio
  Vector    pos_;       // cam position
  Vector    dir_;       // cam direction angles
  
}; // struct GlCameraEuler

}  // namespace anshub

#endif  // GC_GL_CAMERA_EULER_H