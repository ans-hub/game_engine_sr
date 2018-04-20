// *************************************************************
// File:    gl_camera_fol.h
// Descr:   follow type camera
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef GC_GL_CAMERA_FOLLOW_H
#define GC_GL_CAMERA_FOLLOW_H

#include <limits>

#include "lib/draw/gl_camera.h"
#include "lib/draw/gl_object.h"

#include "lib/math/trig.h"
#include "lib/math/vector.h"
#include "lib/math/matrix.h"

namespace anshub {

//***************************************************************************
// FOLLOW CAMERA INTERFACE
//***************************************************************************

struct CameraFol : public GlCamera
{
  CameraFol(float fov, float dov, int scr_w, int scr_h,
    cVector& vrp, cVector& dir, float z_near, float z_far, const TrigTable&
  );
  CameraFol(const CameraFol&) =default;
  CameraFol& operator=(const CameraFol&) =default;
  CameraFol(CameraFol&&) =default;
  CameraFol& operator=(CameraFol&&) =default;
  ~CameraFol() noexcept { }

  void FollowFor(const GlObject&, cVector& vrp_offset, cVector& dir_offset);
  void FollowFor(const GlObject&);
  void Preprocess() override { vrp_ = vrp_orig_; }

private:
  Vector vrp_orig_;   // stores origin vrp for vrp rotating purposes
  Vector obj_dir_;    // stores object direction to track changes 
  Vector obj_pos_;    // stores object position to track changes

}; // struct CameraFol

}  // namespace anshub

#endif  // GC_GL_CAMERA_FOLLOW_H