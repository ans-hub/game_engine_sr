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
#include "trig.h"
#include "exceptions.h"

namespace anshub {

//****************************************************************************
//  ALIASES
//****************************************************************************

struct  Vector;
using   Point = Vector;
using   Vec = Vector;
using   cVec = const Vector;
using   cVector = const Vector;

//****************************************************************************
// VECTOR CLASS DEFINITION
//****************************************************************************

struct Vector
{
  Vector() 
    : x{}, y{}, z{}, w{1.0f} { }
  Vector(const Vector& v1, const Vector& v2)
    : x{v2.x-v1.x}, y{v2.y-v1.y}, z{v2.z-v1.z}, w{1.0} { }
  Vector(float ax, float ay) 
    : x{ax}, y{ay}, z{}, w{1.0} { }
  Vector(float ax, float ay, float az) 
    : x{ax}, y{ay}, z{az}, w{1.0} { }
  
  float x;
  float y;
  float z;
  float w;   // see note #2 after code

  float Length() const;
  float SquareLength() const;  // used to exclude sqrt in some calculations
  void  Normalize();

  void  Zero() { x = 0.0f; y = 0.0f; z = 0.0f; w = 1.0f; }
  bool  IsZero() const;

  Vector& operator*=(float scalar) {
    this->x *= scalar;
    this->y *= scalar;
    this->z *= scalar;
    return *this;
  }
  Vector& operator/=(float scalar) {
    this->x /= scalar;
    this->y /= scalar;
    this->z /= scalar;
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
  friend inline Vector operator*(Vector lhs, float scalar) {
    lhs *= scalar;
    return lhs;
  }
  friend inline Vector operator/(Vector lhs, float scalar) {
    lhs /= scalar;
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

  float   Length(const Vector&);
  float   SquareLength(const Vector&);
  Vector  Add(const Vector&, const Vector&);
  Vector  Sub(const Vector&, const Vector&);
  Vector  Normalize(const Vector&);
  float   DotProduct(const Vector&, const Vector&);
  Vector  CrossProduct(const Vector&, const Vector&);
  bool    IsCollinearA(const Vector&, const Vector&, bool = false);
  bool    IsCollinearB(const Vector&, const Vector&);
  float   CosBetween(const Vector&, const Vector&, bool = false);
  float   AngleBetween(const Vector&, const Vector&, bool = false);
  void    ConvertFromHomogeneous(Vector&);
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

inline float Vector::Length() const
{
  return std::sqrt(std::pow(x, 2) + std::pow(y, 2) + std::pow(z, 2));
}

inline float Vector::SquareLength() const
{
  return std::pow(x, 2) + std::pow(y, 2) + std::pow(z, 2);
}

// Make length == 1

inline void Vector::Normalize()
{
  float len = Length();
  if (math::Fzero(len)) {
    throw MathExcept("vector::Normalize - zero length vector");
  }
  float p = 1 / len;
  x *= p;
  y *= p;
  z *= p;
}

inline bool Vector::IsZero() const
{ 
  return math::Fzero(x) && math::Fzero(y) && math::Fzero(z);
}


//****************************************************************************
// INLINE HELPER FUNCTIONS IMPLEMENTATION
//****************************************************************************

// Simplie divides all components by w

inline void vector::ConvertFromHomogeneous(Vector& v)
{
  v /= v.w;
  v.w = 1.0f;
}


// Returns dot product (scalar) (evaluated by coordinates)
// If result > 0, then angle between v1 && v2 from 0 to 90 degree
// If result < 0, then angle between v1 && v2 more than 90 degree
// 0 - particullary 90

inline float vector::DotProduct(const Vector& v1, const Vector& v2)
{
  return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
}

// Returns vector which is perpendicular to v1 and v2. Only actual for
// 3d vectors

// Properties of this vector are:
//  1) [v1*v2] == -[v2*v1]
//  2) length of result == S parallelogram based on v1 and v2
//  3) S par-m = |v1| * |v2| * sin (v1,v2);

// To get cross product of vector we should solve determinant. We have 8 
// variants to do this. One of them:
//
//        | a1 b1 c1 |     |   i     j     k  |
//  det = | a2 b2 c2 |  =  | v1.x  v1.y  v1.z |
//        | a3 b3 c3 |     | v2.x  v2.y  v2.z |
//
//  solve = a1*b2*c3 + b1*c2*a3 + c1*a2*b3 - c1*b2*a3 - a1*c2*b3 - b1*a2*c3
//          i          j          k          k          i          j

// ru1: https://goo.gl/m9BdbV
// ru2: https://goo.gl/PVm3oX

inline Vector vector::CrossProduct(const Vector& v1, const Vector& v2)
{
  return Vector
  (
    (v1.y * v2.z - v2.y * v1.z),
   -(v1.x * v2.z - v2.x * v1.z),
    (v1.x * v2.y - v2.x * v1.y)    
  );
}

} // namespace anshub

#endif  // GM_VECTOR_H

// Note #1 : operator- as argument has lhs given by value. And operator+ as argument
// has const reference to lhs. In both cases we create new point to return result.
// We should test which case is better in perfomance.

// Note #2: this is fiction coordinate which used only to get capable of multiply mxs.
// One important thing - is in perspective proj. In quaternions w coordinate uses to
// represents really coordinate (it may be multiplie, add, and sub ...)

// Todo #1 : may be do as Vector<2>, Vector<3> ??? Yes, surely
// Todo #2 : make all inline ??? 