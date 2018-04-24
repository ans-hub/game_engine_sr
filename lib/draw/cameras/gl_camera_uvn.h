// *************************************************************
// File:    gl_camera_uvn.h
// Descr:   uvn-camera
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef GC_GL_CAMERA_UVN_H
#define GC_GL_CAMERA_UVN_H

#include <limits>

#include "lib/draw/gl_coords.h"

#include "lib/draw/cameras/gl_camera.h"

#include "lib/math/trig.h"
#include "lib/math/vector.h"
#include "lib/math/matrix.h"
#include "lib/math/matrixes/mx_rotate_uvn.h"

namespace anshub {

//***************************************************************************
// Represents UVN-type camera
//***************************************************************************

namespace camera_const {

  constexpr float kMaxFloat {std::numeric_limits<float>::max()};
  constexpr int kDefaultGimbalSpeed = 15;
  constexpr Vector kDefaultGimbalTarget {kMaxFloat, kMaxFloat, kMaxFloat};

} // namespace camera_const

struct CameraUvn : public GlCamera
{
  struct Gimbal     // Helps to make soft rotation of camera while LookAt()
  {
    Gimbal()
    : steps_{1}
    , speed_{camera_const::kDefaultGimbalSpeed}
    , target_{camera_const::kDefaultGimbalTarget}
    , velocity_{}
    { }
    int     steps_;
    int     speed_;
    Vector  target_;
    Vector  velocity_;

  }; // struct Gimbal

  CameraUvn(float fov, float dov, int scr_w, int scr_h,
    cVector& vrp, cVector& dir, float z_near, float z_far, cTrigTable&
  );
  CameraUvn(const CameraUvn&) =default;
  CameraUvn& operator=(const CameraUvn&) =default;
  CameraUvn(CameraUvn&&) =default;
  CameraUvn& operator=(CameraUvn&&) =default;
  ~CameraUvn() noexcept { }

  void LookAt(const Vector&, float roll_hint = 0.0f);
  void Reinitialize(); // used after camera type changing
  void Preprocess() override { }
  auto GetU() const { return u_; }
  auto GetV() const { return v_; }
  auto GetN() const { return n_; }

private:
  Vector    u_;           //
  Vector    v_;           // camera basis (as x,y,z)
  Vector    n_;           // 
  Vector    view_;        // cam view vector
  Gimbal    gimbal_;      // target point, look-at point

}; // struct CameraUvn

//***************************************************************************
// UVN-camera helpers
//***************************************************************************

namespace camera_helpers {

  Vector ConvertUvn2Euler(cVector&, cVector&, cVector&);

} // namespace camera_helpers

} // namespace anshub

#endif  // GC_GL_CAMERA_UVN_H