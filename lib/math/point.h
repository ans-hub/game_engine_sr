// *************************************************************
// File:    point.h
// Descr:   represents 2d-3d point
// Author:  Novoselov Anton @ 2017-2018
// URL:     https://github.com/ans-hub/geomath_lib
// *************************************************************

#ifndef GM_POINT_2D_3D_H
#define GM_POINT_2D_3D_H

#include "math.h"

namespace anshub {

struct Point
{
  Point() : x{}, y{}, z{} { }
  Point(float ax, float ay) : x{ax}, y{ay}, z{} { }
  Point(float ax, float ay, float az) : x{ax}, y{ay}, z{az} { }

  float x;
  float y;
  float z;
  
  void Zero() { x = float(); y = float(); z = float(); }

  Point& operator*(float scalar) {
    this->x *= scalar;
    this->y *= scalar;
    this->z *= scalar;
    return *this;
  }
  Point& operator+=(const Point& rhs) {
    this->x += rhs.x;
    this->y += rhs.y;
    this->z += rhs.z;
    return *this;
  }
  Point& operator-=(const Point& rhs) {
    this->x -= rhs.x;
    this->y -= rhs.y;
    this->z -= rhs.z;
    return *this;
  }
  bool operator==(const Point& p)
  {
    return (math::Feq(x, p.x)) && (math::Feq(y, p.y)) && (math::Feq(z, p.z));
  }
  bool operator!=(const Point& p)
  {
    return !(*this == p);
  }
  friend inline Point operator-(Point lhs, const Point& rhs)
  {
    lhs -= rhs;
    return lhs;
  }
  friend inline Point operator+(const Point& lhs, const Point& rhs)
  {
    return Point(lhs.x+rhs.x, lhs.y+rhs.y, lhs.z+rhs.z);
  }

}; // struct Point

}  // namespace anshub

#endif  // GM_POINT_2D_3D_H

// Notes : operator- as argument has lhs given by value. And operator+ as argument
// has const reference to lhs. In both cases we create new point to return result.
// We should test which case is better in perfomance.