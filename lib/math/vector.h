// *************************************************************
// File:    vector.h
// Descr:   represents 2d-3d-4d vector
// Author:  Novoselov Anton @ 2017-2018
// URL:     https://github.com/ans-hub/geomath_lib
// *************************************************************

#ifndef GM_VECTOR_H
#define GM_VECTOR_H

#include <iostream>
#include <cmath>

#include "math.h"
#include "exceptions.h"
#include "segment.h"

namespace anshub {

//****************************************************************************
// VECTOR CLASS DEFINITION
//****************************************************************************

struct Vector
{
  Vector() 
    : x{}, y{}, z{} { }
  explicit Vector(const Point& p) 
    : x{p.x}, y{p.y}, z{p.z}, w{1.0} { }
  Vector(const Vector& v1, const Vector& v2)
    : x{v2.x-v1.x}, y{v2.y-v1.y}, z{v2.z-v1.z}, w{1.0} { }
  Vector(const Point& p1, const Point& p2)
    : x{p2.x-p1.x}, y{p2.y-p1.y}, z{p2.z-p1.z}, w{1.0} { }
  Vector(double ax, double ay) 
    : x{ax}, y{ay}, z{}, w{1.0} { }
  Vector(double ax, double ay, double az) 
    : x{ax}, y{ay}, z{az}, w{1.0} { }
  explicit Vector(const Segment& s) 
    : Vector(s.b - s.a) { }
  
  double x;
  double y;
  double z;
  double w;   // see note #2 after code

  double Length() const;
  double SquareLength() const;  // used to exclude sqrt in some calculations
  void   Normalize();

  void Zero() { x = double(); y = double(); z = double(); }

  Vector& operator*=(double scalar) {
    this->x *= scalar;
    this->y *= scalar;
    this->z *= scalar;
    return *this;
  }
  Vector& operator+=(const Vector& rhs) {
    this->x += rhs.x;
    this->y += rhs.y;
    this->z += rhs.z;
    return *this;
  }
  Vector& operator-=(const Vector& rhs) {
    this->x -= rhs.x;
    this->y -= rhs.y;
    this->z -= rhs.z;
    return *this;
  }
  bool operator==(const Vector& v)  // todo : what will be with 2d vector !?
  {
    return (math::Feq(x, v.x)) && (math::Feq(y, v.y)) && (math::Feq(z, v.z));
  }
  bool operator!=(const Vector& v)
  {
    return !(*this == v);
  }
  friend inline Vector operator*(Vector lhs, double scalar) {
    lhs *= scalar;
    return lhs;
  }
  friend inline Vector operator-(Vector lhs, const Vector& rhs) {
    lhs -= rhs;
    return lhs;
  }
  friend inline Vector operator+(const Vector& lhs, const Vector& rhs)
  {
    return Vector(lhs.x+rhs.x, lhs.y+rhs.y, lhs.z+rhs.z);
  }
  friend inline bool operator<(const Vector& lhs, const Vector& rhs)
  {
    return lhs.SquareLength() < rhs.SquareLength();
  }

}; // struct Vector

//****************************************************************************
//  DEFINITION OF HELPERS FUNCTIONS FOR VECTOR CLASS
//****************************************************************************

namespace vector {

  double Length(const Vector&);
  double SquareLength(const Vector&);
  Vector Add(const Vector&, const Vector&);
  Vector Sub(const Vector&, const Vector&);
  Vector Normalize(const Vector&);
  double DotProduct(const Vector&, const Vector&);
  Vector CrossProduct(const Vector&, const Vector&);
  bool   IsCollinearA(const Vector&, const Vector&, bool = false);
  bool   IsCollinearB(const Vector&, const Vector&);
  double CosBetween(const Vector&, const Vector&, bool = false);
  double AngleBetween(const Vector&, const Vector&, bool = false);
  std::ostream& operator<<(std::ostream&, const Vector&);

} // namespace vector

namespace vector2d {

  Vector Rotate90CW(const Vector&);
  void   Rotate90CW(Vector&);
  Vector Rotate90CCW(const Vector&);
  void   Rotate90CCW(Vector&);
  bool   IsCollinear(const Vector&, const Vector&);
  bool   IsOrthogonal(const Vector&, const Vector&);

} // namespace vector2d

//****************************************************************************
// INLINE MEMBER CLASS FUNCTIONS IMPLEMENTATION
//****************************************************************************

inline double Vector::Length() const
{
  return std::sqrt(std::pow(x, 2) + std::pow(y, 2) + std::pow(z, 2));
}

inline double Vector::SquareLength() const
{
  return std::pow(x, 2) + std::pow(y, 2) + std::pow(z, 2);
}

// Make length == 1

inline void Vector::Normalize()
{
  double len = Length();
  if (math::Fzero(len)) {
    throw MathExcept("vector::Normalize - zero length vector");
  }
  double p = 1 / len;
  x *= p;
  y *= p;
  z *= p;
}

} // namespace anshub

#endif  // GM_VECTOR_H

// Note #1 : operator- as argument has lhs given by value. And operator+ as argument
// has const reference to lhs. In both cases we create new point to return result.
// We should test which case is better in perfomance.

// Note #2: this is fiction coordinate which used only to multiply mxs. But really
// in quaternions w coordinate uses to represents really coordinate (it may be multiplie,
// add, and sub ...)

// Todo #1 : may be do as Vector<2>, Vector<3> ??? Yes, surely
// Todo #2 : make all inline ??? 