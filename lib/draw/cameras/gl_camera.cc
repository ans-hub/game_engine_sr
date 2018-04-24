// *************************************************************
// File:    gl_camera.cc
// Descr:   camera class based on Euler`s angles
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#include "gl_camera.h"

namespace anshub {

GlCamera::GlCamera
(
    float fov,
    float dov, 
    int scr_w, 
    int scr_h, 
    const Vector& vrp, 
    const Vector& dir,
    float z_near,
    float z_far,
    const TrigTable& trig
)
  : fov_{fov}
  , dov_{dov}
  , wov_{2 * trig::CalcOppositeCatet(dov_, fov_/2, trig)}
  , z_near_{z_near}
  , z_far_{z_far}
  , scr_w_{scr_w}
  , scr_h_{scr_h}
  , ar_{static_cast<float>(scr_w_) / static_cast<float>(scr_h_)}
  , vrp_{vrp}
  , dir_{dir}
  , type_{CamType::EULER}
  , trig_{trig}
  , pitch_{}
  , yaw_{}
  , roll_{}
{ }

// Computes camera`s view vector (by convient we rotate vertices by YXZ
// sequence, but to compute camera view vector we should multiplie it
// by reverse order ZXY)

Vector camera_helpers::ComputeCamViewVector(cVector& cam_dir, cTrigTable& trig)
{
  // Rotate cameras direction vector in reverse order

  MatrixRotateEul mx_x {cam_dir.x, 0.0f, 0.0f, trig};
  MatrixRotateEul mx_y {0.0f, cam_dir.y, 0.0f, trig};
  MatrixRotateEul mx_z {0.0f, 0.0f, cam_dir.z, trig};
  Matrix<4,4> mx {};
  mx = matrix::Multiplie(mx_z, mx_x);
  mx = matrix::Multiplie(mx, mx_y);
  
  // Find view camera vector

  auto view = matrix::Multiplie(Vector{0.0f, 0.0f, 1.0f}, mx);
  view.Normalize();
  return view;
}

} // namespace anshub