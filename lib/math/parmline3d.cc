// *************************************************************
// File:    parmline3d.cc
// Descr:   represents 3d parametric line
// Author:  Novoselov Anton @ 2017
// *************************************************************

#include "parmline3d.h"

namespace anshub {

// Returns vector (point) on the line that intersects 3d plane

Vector parmline3d::Intersects(const Parmline3d& l, const Plane3d& p)
{
  Vector res{};
  
  // Substitute unknowns in plane equation to parametric components of line
  //  plane: ax + by + cz - d = 0
  //  substitute: a*(x0+dx*t) + b(y0+dy*t) + c(z0+dz*t) - d = 0; 

  // Open braces, collect free members and members contains `t`, and find `t`

  float tfree = p.a_ * l.p0_.x + p.b_ * l.p0_.y + p.c_ * l.p0_.z + p.d_;
  float tcont = p.a_ * l.dx_ + p.b_ * l.dy_ + p.c_ * l.dz_;
  float t = -tfree / tcont;

  // Find point which intersects plane and line
  
  res.x = l.p0_.x + l.dx_*t;
  res.y = l.p0_.y + l.dy_*t;
  res.z = l.p0_.z + l.dz_*t;

  return res;
}

// The same as above but returns intersects point through argument and bool as
// result of searching intersects point
//  todo: additional check to t-range in 0-1

bool parmline3d::Intersects(const Parmline3d& l, const Plane3d& p, Vector& i)
{
  Vector dir    {l.dx_, l.dy_, l.dz_};
  Vector normal {0.0f, 0.0f, 1.0f};
  auto prod = vector::DotProduct(dir, normal);
  
  if (math::FNotZero(prod)) {
    i = parmline3d::Intersects(l, p);
    return true;
  }
  else
    return false;
}

// Returns `t` parameter with which line and plane are would intersects

float parmline3d::FindIntersectsT(const Parmline3d& l, const Plane3d& p)
{
  float tfree = p.a_ * l.p0_.x + p.b_ * l.p0_.y + p.c_ * l.p0_.z + p.d_;
  float tcont = p.a_ * l.dx_ + p.b_ * l.dy_ + p.c_ * l.dz_;
  return -tfree / tcont;
}

} // namespace anshub