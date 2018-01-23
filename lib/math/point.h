// *************************************************************
// File:    point.h
// Descr:   represents point entitie
// Author:  Novoselov Anton @ 2017
// URL:     https://github.com/ans-hub/geomath_lib
// *************************************************************

#ifndef SMPL_POINT_H
#define SMPL_POINT_H

namespace anshub {

struct Point
{
  Point() : x{-1.0}, y{-1.0}, z{-1.0} { }
  Point(double ax, double ay, double az) : x{ax}, y{ay}, z{az} { }
  double x;
  double y;
  double z;
  
  void Zero() { x = 0.0; y = 0.0; z = 0.0; }

  inline Point& operator*(double scalar) {
    this->x *= scalar;
    this->y *= scalar;
    this->z *= scalar;
    return *this;
  }
  friend inline Point operator-(const Point& lhs, const Point& rhs) {
    return Point(lhs.x-rhs.x, lhs.y-rhs.y, lhs.z-rhs.z);
  }
  friend inline Point operator+(const Point& lhs, const Point& rhs) {
    return Point(lhs.x+rhs.x, lhs.y+rhs.y, lhs.z+rhs.z);
  }

}; // struct Point

}  // namespace anshub

#endif  // SMPL_POINT_H