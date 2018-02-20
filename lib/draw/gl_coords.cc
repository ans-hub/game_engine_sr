// *****************************************************************
// File:    gl_coords.h
// Descr:   functions to convert different coordinates (non-marices)
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *****************************************************************

#include "gl_coords.h"

namespace anshub {

// Convertes given vertexes into the world coordinates

void coords::Local2World(Vertexes& vxs, const Vector& move)
{
  for (auto& vx : vxs)
  {
    vx.x += move.x;
    vx.y += move.y;
    vx.z += move.z;
  }
}

// Translate all vertexes into the camera coordinates (by YXZ sequence)

void coords::World2Camera(
  Vertexes& vxs, cVector& cam_pos, cVector& cam_dir, const TrigTable& trig)
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

    vx.y -= cam_pos.y;
    vx.x -= cam_pos.x;
    vx.z -= cam_pos.z;

    // Y-axis rotate (yaw)

    if (math::FNotZero(cam_dir.y))
    {
      float vx_old {vx.x};
      vx.x = (vx.x * ycos) + (vx.z * ysin);
      vx.z = (vx.z * ycos) - (vx_old * ysin); 
    }

    // X-axis rotate (pitch)

    if (math::FNotZero(cam_dir.x))
    {
      float vy_old {vx.y}; 
      vx.y = (vx.y * xcos) - (vx.z * xsin);
      vx.z = (vx.z * xcos) + (vy_old * xsin); 
    }

    // Z-axis rotate (roll)

    if (math::FNotZero(cam_dir.z))
    {
      float vx_old {vx.x};
      vx.x = (vx.x * zcos) - (vx.y * zsin);
      vx.y = (vx.y * zcos) + (vx_old * zsin);
    }
  }
}

// Translates all vertexes from camera (world) to perspective

void coords::Camera2Persp(Vertexes& vxs, float dov, float ar)
{
  for (auto& vx : vxs)
  {
    vx.x *= dov / vx.z;
    vx.y *= dov * ar / vx.z;
  }
}

// Where wov - width of view

void coords::Persp2Screen(Vertexes& vxs, float wov, int scr_w, int scr_h)
{
  // Define proportion koefficients

  float kx = scr_w / wov; // how much pixels in one unit of projection plane 
  float ky = scr_h / wov;
  
  float half_wov = wov / 2;

  // Convert all points from persp to screen

  for (auto& vx : vxs)
  {
    vx.x = (vx.x + half_wov) * kx;   // convert -half_wov +half_wov
    vx.y = (vx.y + half_wov) * ky;   // to 0-width, 0-height
  }
}

// Rotates all vertexes by y-axis (conventionally yaw)

void coords::RotateYaw(Vertexes& vxs, float deg, TrigTable& trig)
{
  float sine_y = trig.Sin(deg);
  float cosine_y = trig.Cos(deg);
  for (auto& vx : vxs)
  {
    vx.x = vx.x * cosine_y + vx.z * sine_y;
    vx.z = vx.x * -sine_y  + vx.z * cosine_y;
  }
}

// Rotates all vertexes by x-axis (conventionally pitch)

void coords::RotatePitch(Vertexes& vxs, float deg, TrigTable& trig)
{
  float sine_x = trig.Sin(deg);
  float cosine_x = trig.Cos(deg);
  for (auto& vx : vxs)
  {
    vx.y = vx.y * cosine_x - vx.z * sine_x;
    vx.z = vx.y * sine_x   + vx.z * cosine_x;
  }
}

// Rotates all vertexes by z-axis (conventionally roll)

void coords::RotateRoll(Vertexes& vxs, float deg, TrigTable& trig)
{
  float sine_z = trig.Sin(deg);
  float cosine_z = trig.Cos(deg);
  for (auto& vx : vxs)
  {
    vx.x = vx.x * cosine_z - vx.y * sine_z;
    vx.y = vx.x * sine_z   + vx.y * cosine_z;
  }
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