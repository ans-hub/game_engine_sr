// *************************************************************
// File:    gl_camera.cc
// Descr:   imitates camera for renderer (euler and uvn)
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
    float z_far
)
  : trig_{}
  , fov_{fov}
  , dov_{dov}
  , wov_{2 * trig::CalcOppositeCatet(dov_, fov_/2, trig_)}
  , z_near_{z_near}
  , z_far_{z_far}
  , scr_w_{scr_w}
  , scr_h_{scr_h}
  , ar_{(float)scr_w_ / (float)scr_h_}
  , vrp_{vrp}
  , dir_{dir}
  , u_{0.0f, 0.0f, 1.0f}
  , v_{0.0f, 1.0f, 1.0f}
  , n_{1.0f, 0.0f, 1.0f}
  , target_{}
{ }

// Set look-at point and recalculate vectors of uvn type camera
// When we use uvn camera, rotating of camera is automated. 
// We should manually refresh directions

void GlCamera::LookAt(const Vector& p, float roll_hint) // hint in degrees
{
  target_ = p;
  n_ = Vector{vrp_, target_};           // direction vector
  
  // Now suppose that v_ is vector "up". Roll_hint fixes z_rot axis. If hint
  // is 0.0f, then v is true directed up. In this case we would have true
  // "right" vector. If hint is != 0, then v is up and just left or right.
  // Now we would have roll by given hint

  v_ = Vector{roll_hint/90.0f, 1.0f, 0.0f};   // suppose vector "up"
  u_ = vector::CrossProduct(v_, n_);          // vector directed "right"
  
  if (u_.SquareLength() == 0)                 // if n_ and u_ are collinear, then
  {                                           // then suppose u_ just another dir
    v_ = {0.0f, 1.0f, 1.0f};
    u_ = vector::CrossProduct(v_, n_); 
  }
  v_ = vector::CrossProduct(n_, u_);          // recalc v
  u_.Normalize();
  v_.Normalize();
  n_.Normalize();

  // Here I was trying to extract euler angles from uvn vectors 
  // by defining anlge between plane and vector

  // Vector z_plane_norm {0.0f, 0.0f, 1.0f};
  // float angle_3 = vector::AngleBetween(z_plane_norm, v_, true) - 90;
  // dir_.x = -angle_3;

  // // Use vector "right" to find angle between xz plane

  // Vector x_plane_norm {1.0f, 0.0f, 0.0f};
  // float angle_2 = vector::AngleBetween(x_plane_norm, n_, true) - 90;
  // dir_.y = -angle_2;

  // // Now calc angle between y plane (z-rot)

  // Vector y_plane_norm {0.0f, 1.0f, 0.0f};
  // float angle = vector::AngleBetween(y_plane_norm, u_, true) - 90;
  // dir_.z = -angle;
}

} // namespace anshub