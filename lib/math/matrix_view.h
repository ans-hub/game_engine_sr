// *************************************************************
// File:    matrix_view.h
// Descr:   viewport matrix
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/geomath_lib
// *************************************************************

#ifndef GM_MATRIX_VIEWPORT_H
#define GM_MATRIX_VIEWPORT_H

#include "matrix.h"
#include "point.h"
#include "vector.h"

namespace anshub {

struct MatrixViewport : public Matrix<4,4>
{
  MatrixViewport(float, int, int);

}; // struct MatrixViewport

inline MatrixViewport::MatrixViewport(float wov, int scr_w, int scr_h)
: Matrix(Type::IDENTITY)
{
  float kx = scr_w / wov;
  float ky = scr_h / wov;
  float half_wov = wov / 2;

  Matrix::Container tmp =
  {
      kx,           0.0f,         0.0f,   0.0f,
      0.0f,         ky,           0.0f,   0.0f,
      0.0f,         0.0f,         1.0f,   1.0f,
      half_wov*kx,  half_wov*ky,  0.0f,   0.0f
  };
  data_ = std::move(tmp);
} 

}  // namespace anshub

#endif  // GM_MATRIX_VIEWPORT_H