// *************************************************************
// File:    matrix_rotate.h
// Descr:   rotate matrix
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef GC_MATRIX_ROTATE_H
#define GC_MATRIX_ROTATE_H

#include "trig.h"
#include "math.h"
#include "vector.h"
#include "matrix.h"

namespace anshub {

struct MatrixRotate : public Matrix<4,4>
{
  MatrixRotate(float, float, float, const TrigTable&);
  MatrixRotate(const Vector&, const TrigTable&);

}; // struct MatrixScale

// Implementation

inline MatrixRotate::MatrixRotate(
  float x, float y, float z, const TrigTable& t)
: Matrix(Type::IDENTITY)
{
  Matrix<4,4> res (Type::IDENTITY);
  if (!math::FNotZero(x))
  {
    float tsin = t.Sin(x);
    float tcos = t.Cos(x);
    Matrix<4,4> xrot = {
      1.0f,   0.0f,   0.0f,   0.0f,
      0.0f,   tcos,   tsin,   0.0f, 
      0.0f,  -tsin,   tcos,   0.0f,
      0.0f,   0.0f,   0.0f,   1.0f 
    };
    res = matrix::Multiplie(res, xrot);
  }
  if (!math::FNotZero(y))
  {
    float tsin = t.Sin(y);
    float tcos = t.Cos(y);
    Matrix<4,4> yrot = {
      tcos,   0.0f,  -tsin,   0.0f,
      0.0f,   1.0f,   0.0f,   0.0f, 
      tsin,   0.0f,   tcos,   0.0f,
      0.0f,   0.0f,   0.0f,   1.0f 
    };
    res = matrix::Multiplie(res, yrot);    
  }
  if (!math::FNotZero(z))
  {
    float tsin = t.Sin(z);
    float tcos = t.Cos(z);
    Matrix<4,4> zrot = {
      tcos,   tsin,   0.0f,   0.0f,
     -tsin,   tcos,   0.0f,   0.0f, 
      0.0f,   0.0f,   1.0f,   0.0f,
      0.0f,   0.0f,   0.0f,   1.0f 
    };
    res = matrix::Multiplie(res, zrot);    
  }
  data_ = std::move(res.Data());
}

inline MatrixRotate::MatrixRotate(const Vector& ang, const TrigTable& t)
: MatrixRotate(ang.x, ang.y, ang.z, t)
{ }

}  // namespace anshub

#endif  // GC_MATRIX_ROTATE_H