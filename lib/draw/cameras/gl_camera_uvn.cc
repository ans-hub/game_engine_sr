// *************************************************************
// File:    gl_camera_uvn.cc
// Descr:   uvn-camera
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#include "gl_camera_uvn.h"

namespace anshub {

CameraUvn::CameraUvn(
  float fov,
  float dov,
  int scr_w,
  int scr_h,
  cVector& vrp,
  cVector& dir,
  float z_near,
  float z_far,
  const TrigTable& trig
)
  : GlCamera(fov, dov, scr_w, scr_h, vrp, dir, z_near, z_far, trig)
  , u_{1.0f, 0.0f, 0.0f}
  , v_{0.0f, 1.0f, 0.0f}
  , n_{0.0f, 0.0f, 1.0f}
  , view_{0.0f, 0.0f, 1.0f}
  , gimbal_{}
{
  type_ = CamType::UVN;
  Reinitialize();
}

// Refresh view vector of camera. We use this function after switchin camera
// type from any to uvn, i.e. from cameraman class

void CameraUvn::Reinitialize()
{
  gimbal_ = Gimbal();
  view_ = camera_helpers::ComputeCamViewVector(dir_, trig_);
}

// Set look-at point and recalculate vectors of uvn type camera
// When we use uvn camera, rotating of camera is automated. 
// We should manually refresh directions

void CameraUvn::LookAt(const Vector& p, float roll_hint) // hint in degrees
{
  // 1. This section of code is define vector from vrp_ to target point. In 
  // regular uvn camera we call this vector as n_, and go to the section 2. 
  // But here we want to make soft motion of camera, when we choose some 
  // object which we don`t focus on early.

  // At the end of movement we make gimbal speed == 1, since if we plane 
  // just observe target object, then we don`t need soft speed - we need
  // fixed hard linking with target point without depending of speed of obj.

  if (gimbal_.target_ != p)
  {
    gimbal_.target_ = p;
    Vector aim (vrp_, gimbal_.target_);
    aim.Normalize();
    Vector move (view_, aim);
    gimbal_.velocity_ = move / gimbal_.speed_;
    gimbal_.steps_ = gimbal_.speed_;
    view_+= gimbal_.velocity_;
    n_ = view_;
    --gimbal_.steps_;
  }
  else if (gimbal_.steps_ != 0)
  {
    view_ += gimbal_.velocity_;
    n_ = view_;
    --gimbal_.steps_;
  }
  else
  {
    view_ += gimbal_.velocity_;
    gimbal_.speed_ = 1;
    n_ = Vector(vrp_, gimbal_.target_);
    view_ = vector::Normalize(n_);
  }
  
  // 2. Now suppose that v_ is vector "up". Roll_hint fixes z_rot axis. If hint
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

  // 3. In the end we synchronize uvn vectors and Euler`s direction angle

  dir_ = camera_helpers::ConvertUvn2Euler(u_, v_, n_);
}

// Converts unv vectors to directional Euler`s angles

Vector camera_helpers::ConvertUvn2Euler(cVector& u, cVector& v, cVector& n)
{
  MatrixRotateUvn mx {u, v, n};
  return coords::RotationMatrix2Euler(mx);
}

} // namespace anshub