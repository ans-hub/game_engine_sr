// *************************************************************
// File:    gl_camera_fol.cc
// Descr:   follow type camera
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#include "gl_camera_fol.h"

namespace anshub {

CameraFol::CameraFol(
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
  , vrp_orig_{vrp}
  , obj_dir_{}
  , obj_pos_{}
  , yaw_{}
  , pitch_{}
  , roll_{}
{
  type_ = CamType::FOLLOW;
}

// Initializes camera in object z-orientation with given offset

void CameraFol::FollowFor(
  const GlObject& obj, cVector& vrp_offset, cVector& dir_offset)
{
  // Make vrp z offset

  auto dz = obj.v_orient_z_ * (vrp_offset.z);
  vrp_ = obj.world_pos_ + dz;

  // Make vrp y offset

  vrp_.y += vrp_offset.y;
  vrp_orig_ = vrp_;

  // Initialize dir with given offset

  dir_ = obj.dir_;
  dir_ += dir_offset;
  
  // Store track variables

  obj_dir_ = obj.dir_;
  obj_pos_ = obj.world_pos_;
}

// Continue follow for object

void CameraFol::FollowFor(const GlObject& obj)
{
  // Track direction changes

  auto diff_dir = obj.dir_ - obj_dir_;
  auto diff_vrp = obj.world_pos_ - obj_pos_;

  // Apply direction changes

  if (!pitch_.locked_)
  {
    dir_.x += diff_dir.x / pitch_.reduce_;
    dir_.x = std::max(pitch_.low_, dir_.x);
    dir_.x = std::min(pitch_.high_, dir_.x);
  }
  if (!roll_.locked_)
  {
    dir_.z += diff_dir.z / roll_.reduce_;
    dir_.z = std::max(roll_.low_, dir_.z);
    dir_.z = std::min(roll_.high_, dir_.z);
  }
  if (!yaw_.locked_)
  {
    dir_.y += diff_dir.y / yaw_.reduce_;
    dir_.y = std::max(yaw_.low_, dir_.y);
    dir_.y = std::min(yaw_.high_, dir_.y);
  }

  // Apply vrp changes

  vrp_ += diff_vrp;

  // Store valuse for next frame

  obj_dir_ = obj.dir_;
  obj_pos_ = obj.world_pos_; 
  vrp_orig_ = vrp_; 

  // Rotate vrp

  auto obj2cam_orig = vrp_ - obj.world_pos_; 
  auto obj2cam_rot = obj2cam_orig;

  coords::RotatePitch(obj2cam_rot, obj.dir_.x, trig_);
  coords::RotateYaw(obj2cam_rot, obj.dir_.y, trig_);
  coords::RotateRoll(obj2cam_rot, obj.dir_.z, trig_);
  auto obj2cam_new = obj2cam_rot - obj2cam_orig;

  vrp_ += obj2cam_new;
}

} // namespace anshub

// Note #1 : we need to store previous vrp since every frame we rotate
// object from 0;0;0 (local) to direction. Therefore we should rotate vrp
// every frame to the global changed dir