// *************************************************************
// File:    matrix.cc
// Descr:   represents matrix
// Author:  Novoselov Anton @ 2017-2018
// URL:     https://github.com/ans-hub/geomath_lib
// *************************************************************

#include "matrix.h"

namespace anshub {

double matrix::Determinant(const Matrix<2,2>& mx)
{
  return (
    mx(0,0) * mx(1,1) - mx(0,1) * mx(1,0)
  );
}

double matrix::Determinant(const Matrix<3,3>& mx)
{
  return (
    mx(0,0) * (mx(1,1) * mx(2,2) - mx(2,1) * mx(1,2)) - 
    mx(0,1) * (mx(1,0) * mx(2,2) - mx(2,0) * mx(1,2)) + 
    mx(0,2) * (mx(1,0) * mx(2,1) - mx(2,0) * mx(1,1))
  );
}

// Returns invert matrix

// Result matrix given through the reference due to avoid using 
// exceptions if determinant is 0 (when inverse matrix is absent)

bool matrix::Inverse(const Matrix<2,2>& mx, Matrix<2,2>& res)
{
  double det = matrix::Determinant(mx);
  double inv = 1 / det;

  if (!math::Fzero(det))
  {
    res(0,0) =  mx(1,1) * inv;
    res(0,1) = -mx(0,1) * inv;
    res(1,0) = -mx(1,0) * inv;
    res(1,1) =  mx(0,0) * inv;
    return true;
  }
  else
    return false;
}

// The same as above but for 3x3 matrix

bool matrix::Inverse(const Matrix<3,3>& mx, Matrix<3,3>& res)
{
  double det = matrix::Determinant(mx);
  double inv = 1 / det;

  if (!math::Fzero(det))
  {
    res(0,0) = (mx(1,1) * mx(2,2) - mx(2,1) * mx(1,2)) * inv;
    res(0,1) = (mx(0,1) * mx(2,2) - mx(2,1) * mx(0,2)) * (-inv);
    res(0,2) = (mx(0,1) * mx(1,2) - mx(1,1) * mx(0,2)) * inv;

    res(1,0) = (mx(1,0) * mx(2,2) - mx(2,0) * mx(1,2)) * (-inv);
    res(1,1) = (mx(0,0) * mx(2,2) - mx(2,0) * mx(0,2)) * inv;
    res(1,2) = (mx(0,0) * mx(1,2) - mx(1,0) * mx(0,2)) * (-inv);

    res(2,0) = (mx(1,0) * mx(2,1) - mx(2,0) * mx(1,1)) * inv;
    res(2,1) = (mx(0,0) * mx(2,1) - mx(2,0) * mx(0,1)) * (-inv);
    res(2,2) = (mx(0,0) * mx(1,1) - mx(1,0) * mx(0,1)) * inv;
    return true;
  }
  else
    return false;
}

// Interpret vector as 2d vector with ficted w=1 (which real is unused z)

Vector matrix::Multilplie(const Vector& v, const Matrix<3,3>& mx)
{
  Vector res;
  res.x = v.x * mx(0,0) + v.y * mx(1,0) + v.w * mx(2,0);
  res.y = v.x * mx(0,1) + v.y * mx(1,1) + v.w * mx(2,1);
  res.w = v.x * mx(0,2) + v.y * mx(1,2) + v.w * mx(2,2);
  return res;
}

// Interpret vector as 3d vector with ficted w=1 (which is real absent)

Vector matrix::Multilplie(const Vector& v, const Matrix<4,4>& mx)
{
  Vector res;
  res.x = v.x * mx(0,0) + v.y * mx(1,0) + v.z * mx(2,0) + v.w * mx(3,0);
  res.y = v.x * mx(0,1) + v.y * mx(1,1) + v.y * mx(2,1) + v.w * mx(3,1);
  res.z = v.x * mx(0,2) + v.y * mx(1,2) + v.z * mx(2,2) + v.w * mx(3,2);
  res.w = v.x * mx(0,3) + v.y * mx(1,3) + v.z * mx(2,3) + v.w * mx(3,3);
  return res;
}

} // namespace anshub