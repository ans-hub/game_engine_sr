// *************************************************************
// File:    vector.cc
// Descr:   represents 2d-3d-4d vector
// Author:  Novoselov Anton @ 2017-2018
// URL:     https://github.com/ans-hub/geomath_lib
// *************************************************************

#include "vector.h"

namespace anshub {

//**********************************************************************
// Helpers functions implementation
//**********************************************************************

// Evaluate length as hypotenuse by Pithagorean theorem

float vector::Length(const Vector& v)
{
  return std::sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}

// The same as above but just square of hypotenuse

float vector::SquareLength(const Vector& v)
{
  return (v.x * v.x) + (v.y * v.y) + (v.z * v.z);
}

// Simple addition of two vectors (similar to operator+)

Vector vector::Add(const Vector& v1, const Vector& v2)
{
  return Vector(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

// Simple substraction of two vectors (similar to operator-)

Vector vector::Sub(const Vector& v1, const Vector& v2)
{
  return Vector(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

// Get normalized version of vector (length == 1) by dividing
// all elements by the current length 

Vector vector::Normalize(const Vector& v)
{
  if (!v.IsNormalized())
  {
    float length = v.Length();    
    if (math::Fzero(length))
      throw MathExcept("vector::Normalize - zero length vector");

    float p = 1.0f / length;
    return Vector(v.x * p, v.y * p, v.z * p);
  }
  else
    return v;
}

// Calculates cosine of the angle between given vectors

//   DotProduct(a*b) = |a|*|b|*cos(a;b) 
//   => cos(a;b) = (a*b)/|a|*|b|
// If `norm` == true - supposed that v1 and v2 is normalized (has length == 1)

// Note : since Length() is very expensive, we have two ways:
//  1) don't use this function and think about another way of problem
//  2) prenormalize vectors and use CosBetweenNormalized (which is DotProduct)

float vector::CosBetween(const Vector& v1, const Vector& v2, bool norm)
{
  if (norm)
    return vector::DotProduct(v1, v2);

  float l1 = v1.Length();
  float l2 = v2.Length();
  if (math::Fzero(l1) || math::Fzero(l2))
    throw MathExcept("vector::CosBetween - zero length vector");
  else
    return vector::DotProduct(v1, v2) / (v1.Length() * v2.Length());
}

// Calculate angle between vectors (costs, if vectors are not normalized)

float vector::AngleBetween(const Vector& v1, const Vector& v2, bool norm)
{
  float cosine = vector::CosBetween(v1, v2, norm);
  return trig::Rad2deg( std::acos(cosine) );
}

// Checks if two vectors are collinear (type A)

//   Based on formulas below:  a*b = |a|*|b|*cos(a;b)
//   When cos 0 = 1, => a*b == |a|*|b|*1
//   => (a*b)^2 == |a|^2 & |b|^2
// If `norm` == true - supposed that v1 and v2 is normalized (has length == 1)

bool vector::IsCollinearA(const Vector& v1, const Vector& v2, bool norm)
{
  if (norm)
    return math::Feq(vector::DotProduct(v1, v2), 1.0);
  else
    return math::Feq(
      std::pow(vector::DotProduct(v1, v2), 2),
      v1.SquareLength() * v2.SquareLength()
    );
}

// Checks if two vectors are collinear (type B)

//   Based on formulas below:  a x b = |a|*|b|*sin(a;b).
//   When sin 0 = 0, => a x b = |a|*|b|*0 == 0

bool vector::IsCollinearB(const Vector& v1, const Vector& v2)
{
  Vector v = vector::CrossProduct(v1, v2);
  return math::Fzero(v.x) && math::Fzero(v.y) && math::Fzero(v.z);
}

std::ostream& vector::operator<<(std::ostream& oss, const Vector& v)
{
  oss << "x: " << v.x << "  y: " << v.y << "  z:" << v.z;
  return oss;
}

//**********************************************************************
// Helpers functions for 2d vectors (they no means in 3d)
//**********************************************************************

Vector vector2d::Rotate90CW(const Vector& v)
{
  return Vector(v.y, -v.x);
}

Vector vector2d::Rotate90CCW(const Vector& v)
{
  return Vector(-v.y, v.x);
}

void vector2d::Rotate90CW(Vector& v)
{
  auto old_x = v.x;
  v.x = v.y;
  v.y = -old_x;
}

void vector2d::Rotate90CCW(Vector& v)
{
  auto old_y = v.y;
  v.y = v.x;
  v.x = -old_y;
}

bool vector2d::IsCollinear(const Vector& v1, const Vector& v2)
{
  if (math::Fzero(v1.x) && math::Fzero(v2.x))
    return true;
  if (math::Fzero(v1.y) && math::Fzero(v2.y))
    return true;
  return vector::IsCollinearA(v1, v2);
}

// Checks if two 2d vectors are orhogonals (perpendicular)

bool vector2d::IsOrthogonal(const Vector& v1, const Vector& v2)
{
  return math::Fzero(vector::DotProduct(v1, v2));
}

} // namespace anshub
