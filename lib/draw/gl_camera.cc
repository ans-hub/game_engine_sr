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

void GlCamera::LookAt(const Vector& p)
{
  target_ = p;
  n_ = Vector{vrp_, target_};           // direction vector
  
  // Now suppose that v_ is vector "up". Special case when vector
  // n_ and v_ is collinear (i.e. we above the object in point 0;y;z<0)

  if (n_.z >= 0 && n_.x != 0)
    v_ = Vector{0.0f, 1.0f, 0.0f};
  // else
    // v_ = Vector{0.0f, -1.0f, 0.0f};  // suppose that v is "up" directed  
  u_ = vector::CrossProduct(v_, n_);    // vector directed "right"
  if (u_.SquareLength() == 0)           // if n_ and u_ are collinear, then
  {                                     // then suppose u_ just another dir
    v_ = {0.0f, 1.0f, 1.0f};
    u_ = vector::CrossProduct(v_, n_); 
  }
  v_ = vector::CrossProduct(n_, u_);    // recalc v
  u_.Normalize();
  v_.Normalize();
  n_.Normalize();
}

} // namespace anshub