// *************************************************************
// File:    matrix_camera.h
// Descr:   camera matrix
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef GC_MATRIX_CAMERA_H
#define GC_MATRIX_CAMERA_H

#include "matrix.h"

namespace anshub {

struct MatrixCamera : public Matrix<4,4>
{
  MatrixCamera(const Vector&);

}; // struct MatrixScale

inline MatrixCamera::MatrixCamera(const Vector& v)
: Matrix
  ({
      1.0f, 0.0f, 0.0f, 0.0f,
      0.0f, 1.0f, 0.0f, 0.0f,
      0.0f, 0.0f, 1.0f, 0.0f,
      -v.x, -v.y, -v.z, 1.0f
  })
{ }

}  // namespace anshub

#endif  // GC_MATRIX_CAMERA_H