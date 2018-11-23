// *************************************************************
// File:    matrix_camera.h
// Descr:   row-based camera matrix
// Author:  Novoselov Anton @ 2017
// *************************************************************

#ifndef GC_MATRIX_CAMERA_H
#define GC_MATRIX_CAMERA_H

#include "../matrix.h"

namespace anshub {

// This is simple identity matrix, which will be used to store
// combination of inverted camera translations and rotations.

struct MatrixCamera : public Matrix<4,4>
{
  MatrixCamera();
  MatrixCamera(const Matrix<4,4>&);
  MatrixCamera(const MatrixCamera&);
  MatrixCamera& operator=(const Matrix<4,4>&);
  MatrixCamera& operator=(const MatrixCamera&);

}; // struct MatrixCamera

inline MatrixCamera::MatrixCamera()
: Matrix
  ({
      1.0f, 0.0f, 0.0f, 0.0f,
      0.0f, 1.0f, 0.0f, 0.0f,
      0.0f, 0.0f, 1.0f, 0.0f,
      0.0f, 0.0f, 0.0f, 1.0f
  })
{ }

inline MatrixCamera::MatrixCamera(const Matrix<4,4>& old)
: Matrix<4,4>{}
{
  data_ = old.Data();
}

inline MatrixCamera& MatrixCamera::operator=(const Matrix<4,4>& old)
{
  this->data_ = old.Data();
  return *this;
}

inline MatrixCamera::MatrixCamera(const MatrixCamera& old)
: Matrix<4,4>{}
{
  data_ = old.Data();
}

inline MatrixCamera& MatrixCamera::operator=(const MatrixCamera& old)
{
  this->data_ = old.Data();
  return *this;
}

}  // namespace anshub

#endif  // GC_MATRIX_CAMERA_H