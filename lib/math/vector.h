// *************************************************************
// File:    vector.h
// Descr:   represents 3d vector
// Author:  Novoselov Anton @ 2017
// URL:     https://github.com/ans-hub/geomath_lib
// *************************************************************

#ifndef GM_VECTOR_H
#define GM_VECTOR_H

#include "point.h"
#include "segment.h"

namespace anshub {

struct Vec3d : public Point
{
  Vec3d() : Point() { }
  explicit Vec3d(const Point& p)
    : Point(p) { }
  explicit Vec3d(const Segment& s)
    : Point(s.b.x - s.a.x, s.b.y - s.a.y, s.b.z - s.a.z) { }

  inline double Length() const;
  inline void   Normalize();
  inline void   Rotate(double angle);
  inline Vec3d  Dot(const Vec3d&);
  inline Vec3d  Cross(const Vec3d&);

  // operator +, -, * - is inherited from Point

}; // struct Vec3d

namespace vector {

  double  Length(const Vec3d&);
  Vec3d   Add(const Vec3d&, const Vec3d&);
  Vec3d   Sub(const Vec3d&, const Vec3d&);
  Vec3d   GetNormal(const Vec3d&);
  Vec3d   Normalize(const Vec3d&);
  Vec3d   Rotate(const Vec3d&, double angle);
  Vec3d   CrossProduct(const Vec3d&, const Vec3d&);
  Vec3d   DotProduct(const Vec3d&, const Vec3d&);

} // namespace vector  

} // namespace anshub

#endif  // GM_VECTOR_H