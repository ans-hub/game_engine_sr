// *************************************************************
// File:    plane3d.h
// Descr:   represents 3d plane entity
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/geomath_lib
// *************************************************************

#ifndef GM_PLANE_3D_H
#define GM_PLANE_3D_H

namespace anshub {

//*************************************************************************
// Represents 3d plane as equation ax + by + cz + d = 0
//*************************************************************************

struct Plane3d
{
  Plane3d(float ka, float kb, float kc, float kd)
  : a_{ka}
  , b_{kb}
  , c_{kc}
  , d_{kd} { }

  float a_;
  float b_;
  float c_;
  float d_;

}; // struct Plane3d

} // namespace anshub

#endif  // GM_PLANE_3D_H