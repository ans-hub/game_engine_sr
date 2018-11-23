// *************************************************************
// File:    parmline3d.h
// Descr:   represents 3d parmline entity
// Author:  Novoselov Anton @ 2017
// *************************************************************

#ifndef GM_PARMLINE_3D_H
#define GM_PARMLINE_3D_H

#include "vector.h"
#include "segment.h"
#include "math.h"
#include "plane3d.h"

namespace anshub {
  
//*************************************************************************
// Represents 3d line using parametric form  
//  ax + by + cz + d = 0
//*************************************************************************

struct Parmline3d
{  
  Vector p0_;
  Vector p1_;
  float dx_;
  float dy_;
  float dz_;

  Parmline3d(const Vector& p0, const Vector& p1)
    : p0_{p0}
    , p1_{p1}
    , dx_{p1.x - p0.x}
    , dy_{p1.y - p0.y}
    , dz_{p1.z - p0.z} { }

  float EvaluateX(float t) { return p0_.x + dx_*t; }
  float EvaluateY(float t) { return p0_.y + dy_*t; }
  float EvaluateZ(float t) { return p0_.z + dz_*t; }

}; // struct Parmline3d

//*************************************************************************
// Helpers functions declaration
//*************************************************************************

namespace parmline3d
{
  Vector Intersects(const Parmline3d&, const Plane3d&);
  bool   Intersects(const Parmline3d&, const Plane3d&, Vector&);
  float  FindIntersectsT(const Parmline3d&, const Plane3d&);

} // namespace parmline3d

} // namespace anshub

#endif  // GM_PARMLINE_3D_H