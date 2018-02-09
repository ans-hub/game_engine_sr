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

// Set look-at point and recalculate uvn vectors

void GlCamera::LookAt(const Vector& p)
{
  target_ = p;
  n_ = Vector{vrp_, target_};         // direction vector from vpr to target
  v_ = Vector{0.0f, 1.0f, 0.0f};      // suppose that v is up directed
  u_ = vector::CrossProduct(v_, n_);  // now we have vector directed "right"
  v_ = vector::CrossProduct(n_, u_);  // recalc v, since now we have plane by n and u
  u_.Normalize();
  v_.Normalize();
  n_.Normalize();
}

} // namespace anshub