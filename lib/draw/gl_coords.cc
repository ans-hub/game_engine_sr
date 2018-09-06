// *****************************************************************
// File:    gl_coords.cc
// Descr:   functions to convert different coordinates
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *****************************************************************

#include "gl_coords.h"

namespace anshub {

// Convertes given vertexes into the world coordinates

void coords::Local2World(V_Vertex& vxs, const Vector& move)
{
  for (Vertex& vx : vxs)
    vx.pos_ += move;
}

// Translate all vertexes into the camera coordinates (by YXZ sequence)

void coords::World2Camera(
  V_Vertex& vxs, cVector& cam_pos, cVector& cam_dir, const TrigTable& trig)
{
  float ysin = trig.Sin(-cam_dir.y);
  float ycos = trig.Cos(-cam_dir.y);
  float xsin = trig.Sin(-cam_dir.x);
  float xcos = trig.Cos(-cam_dir.x);
  float zsin = trig.Sin(-cam_dir.z);
  float zcos = trig.Cos(-cam_dir.z);
  
  for (auto& vx : vxs)
  {
    // Translate position

    vx.pos_ -= cam_pos;

    // Y-axis rotate (yaw)

    if (math::FNotZero(cam_dir.y))
    {
      float vx_old {vx.pos_.x};
      vx.pos_.x = (vx.pos_.x * ycos) + (vx.pos_.z * ysin);
      vx.pos_.z = (vx.pos_.z * ycos) - (vx_old * ysin); 
    }

    // X-axis rotate (pitch)

    if (math::FNotZero(cam_dir.x))
    {
      float vy_old {vx.pos_.y}; 
      vx.pos_.y = (vx.pos_.y * xcos) - (vx.pos_.z * xsin);
      vx.pos_.z = (vx.pos_.z * xcos) + (vy_old * xsin); 
    }

    // Z-axis rotate (roll)

    if (math::FNotZero(cam_dir.z))
    {
      float vx_old {vx.pos_.x};
      vx.pos_.x = (vx.pos_.x * zcos) - (vx.pos_.y * zsin);
      vx.pos_.y = (vx.pos_.y * zcos) + (vx_old * zsin);
    }
  }
}

// Translates vector into the camera coordinates (by YXZ sequence)

void coords::World2Camera(
  Vector& vec, cVector& cam_pos, cVector& cam_dir, const TrigTable& trig)
{
  vec += cam_pos * (-1.0f);
  coords::RotateYaw(vec, -cam_dir.y, trig);
  coords::RotatePitch(vec, -cam_dir.x, trig);
  coords::RotateRoll(vec, -cam_dir.z, trig);
}

// Translates all vertexes from camera (world) to perspective

void coords::Camera2Persp(V_Vertex& vxs, float dov, float ar)
{
  for (auto& vx : vxs)
  {
    vx.pos_.x = vx.pos_.x * dov / vx.pos_.z;
    vx.pos_.y = vx.pos_.y * dov * ar / vx.pos_.z;
  }
}

// Translates vector from camera (world) to perspective

void coords::Camera2Persp(Vector& vec, float dov, float ar)
{
  vec.x = vec.x * dov / vec.z;
  vec.y = vec.y * dov * ar / vec.z;
}

// Translates persective coordinates to screen, where wov - width of view

void coords::Persp2Screen(V_Vertex& vxs, float wov, int scr_w, int scr_h)
{
  // Define proportion koefficients

  float kx = scr_w / wov; // how much pixels in one unit of projection plane 
  float ky = scr_h / wov;
  
  float half_wov = wov / 2;

  // Convert all points from persp to screen

  for (auto& vx : vxs)
  {
    vx.pos_.x = (vx.pos_.x + half_wov) * kx;   // convert -half_wov +half_wov
    vx.pos_.y = (vx.pos_.y + half_wov) * ky;   // to 0-width, 0-height
  }
}

// The same as above but for single Vector

void coords::Persp2Screen(Vector& vec, float wov, int scr_w, int scr_h)
{
  // Define proportion koefficients

  float kx = scr_w / wov; // how much pixels in one unit of projection plane 
  float ky = scr_h / wov;
  
  float half_wov = wov / 2;

  // Convert vector from persp to screen

  vec.x = (vec.x + half_wov) * kx;   // convert -half_wov +half_wov
  vec.y = (vec.y + half_wov) * ky;   // to 0-width, 0-height
}

// Clip vector by near z plane

void coords::ClipNearZ(Vector& vec, float near_z)
{
  vec.z = vec.z <= near_z ? near_z : vec.z;
}

// Rotates all vertexes by y-axis (conventionally yaw)

void coords::RotateYaw(V_Vertex& vxs, float deg, TrigTable& trig)
{
  float ysin = trig.Sin(deg);
  float ycos = trig.Cos(deg);
  for (auto& vx : vxs)
  {
    float vx_old {vx.pos_.x};    
    vx.pos_.x = (vx.pos_.x * ycos) + (vx.pos_.z * ysin);
    vx.pos_.z = (vx.pos_.z * ycos) - (vx_old * ysin);
  }
}

// Rotates all vertexes by x-axis (conventionally pitch)

void coords::RotatePitch(V_Vertex& vxs, float deg, TrigTable& trig)
{
  float xsin = trig.Sin(deg);
  float xcos = trig.Cos(deg);
  for (auto& vx : vxs)
  {
    float vy_old {vx.pos_.y}; 
    vx.pos_.y = (vx.pos_.y * xcos) - (vx.pos_.z * xsin);
    vx.pos_.z = (vx.pos_.z * xcos) + (vy_old * xsin);
  }
}

// Rotates all vertexes by z-axis (conventionally roll)

void coords::RotateRoll(V_Vertex& vxs, float deg, TrigTable& trig)
{
  float zsin = trig.Sin(deg);
  float zcos = trig.Cos(deg);
  for (auto& vx : vxs)
  {
    float vx_old {vx.pos_.x};
    vx.pos_.x = (vx.pos_.x * zcos) - (vx.pos_.y * zsin);
    vx.pos_.y = (vx.pos_.y * zcos) + (vx_old * zsin);
  }
}

// Rotates vector by y-axis (conventionally yaw)

void coords::RotateYaw(Vector& vx, float deg, const TrigTable& trig)
{
  float ysin = trig.Sin(deg);
  float ycos = trig.Cos(deg);

  float vx_old {vx.x};    
  vx.x = (vx.x * ycos) + (vx.z * ysin);
  vx.z = (vx.z * ycos) - (vx_old * ysin);
}

// Rotates vector by x-axis (conventionally pitch)

void coords::RotatePitch(Vector& vx, float deg, const TrigTable& trig)
{
  float xsin = trig.Sin(deg);
  float xcos = trig.Cos(deg);

  float vy_old {vx.y}; 
  vx.y = (vx.y * xcos) - (vx.z * xsin);
  vx.z = (vx.z * xcos) + (vy_old * xsin);
}

// Rotates vector by z-axis (conventionally roll)

void coords::RotateRoll(Vector& vx, float deg, const TrigTable& trig)
{
  float zsin = trig.Sin(deg);
  float zcos = trig.Cos(deg);

  float vx_old {vx.x};
  vx.x = (vx.x * zcos) - (vx.y * zsin);
  vx.y = (vx.y * zcos) + (vx_old * zsin);
}

// Extracts eulers angles from yxz rotation matrix. As example, but in another
// matrixes (row based) - https://goo.gl/AjhkFN

Vector coords::RotationMatrix2Euler(const MatrixRotateUvn& mx)
{
  float yaw {};     // y
  float pitch {};   // x
  float roll {};    // z

  // In depends of gimbal locked or not we have several ways

  float sin_x = mx(1,2);

  // a. If gimbal not locked
  
  if (sin_x > -1.0f && sin_x < 1.0f)
  {
    pitch = std::asin(mx(1,2));             // +sin_x
    yaw   = std::atan2(-mx(0,2), mx(2,2));  // -cos_x*sin_y / cos_x*cos_y
    roll  = std::atan2(-mx(1,0), mx(1,1));  // -cos_x*sin_z / cos_x*cos_z
  }

  // b. If gimbal is locked and look up

  else if (sin_x <= -1.0f) {
    pitch = +math::kPI * 0.5f;              // +90 (if in degrees)
    yaw   = -std::atan2(mx(0,1), -mx(2,1));
    roll  = 0.0f;
  }
  
  // c. If gimbal is locked and look down

  else if (sin_x >= +1.0f) {
    pitch = -math::kPI * 0.5f;              // -90 (if in degrees)
    yaw   = +std::atan2(mx(0,1), -mx(2,1));
    roll  = 0.0f;
  }

  // Finally, convert to degrees and clamp to range 0..360 degrees

  pitch = -trig::Rad2deg(pitch);
  yaw   = -trig::Rad2deg(yaw);
  roll  = -trig::Rad2deg(roll);

  return {pitch, yaw, roll};
}

} // namespace anshub