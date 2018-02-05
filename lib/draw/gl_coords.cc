// *****************************************************************
// File:    gl_coords.h
// Descr:   functions to convert different coordinates (non-marices)
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *****************************************************************

#include "gl_coords.h"

namespace anshub {

Vertexes coord::Local2World(const Vertexes& vxs, const Vector& move)
{
  Vertexes res {};
  res.reserve(vxs.size());

  for (auto& vx : vxs)
  {
    float x = vx.x + move.x;
    float y = vx.y + move.y;
    float z = vx.z + move.z;
    res.emplace_back(x, y, z);
  }
  return res;
}

Vertexes coord::World2Camera(const Vertexes& vxs)
{
  return Vertexes();
}

Vertexes coord::Camera2Persp(const Vertexes& vxs, float dov, float ar)
{
  Vertexes res {};
  res.reserve(vxs.size());

  for (const auto& vx : vxs)
  {
    float x = vx.x * dov / vx.z;
    float y = vx.y * dov * ar / vx.z;
    res.emplace_back(x, y, vx.z);
  }
  return res;
}

// Where wov - width of view

Vertexes coord::Persp2Screen(const Vertexes& vxs, float wov, int scr_w, int scr_h)
{
  // Define proportion koefficients

  float kx = scr_w / wov; // how much pixels in one unit of projection plane 
  float ky = scr_h / wov;
  
  float half_wov = wov / 2;
  Vertexes res {};

  // Convert all points from persp to screen

  res.reserve(vxs.size());
  for (const auto& vx : vxs)
  {
    res.emplace_back(
      (vx.x + half_wov) * kx,   // convert -half_wov +half_wov
      (vx.y + half_wov) * ky,   // to 0-width, 0-height
      0);
  }
  return res;
}

} // namespace anshub