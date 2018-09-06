// *************************************************************
// File:    matrix_camera.h
// Descr:   row-based camera matrix
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef GC_MATRIX_CAMERA_H
#define GC_MATRIX_CAMERA_H

#include "../matrix.h"

namespace anshub {

// This is simple identity matrix, which in fither will be used to
// store combination of inverted camera translations and rotations.

// Copy constructor and assigment implemented by hands since usually
// multiply is provided with Matrix<4,4>, not only MatrixCamera 

// Since copy ctor is implemented explicit, implicit move constructor
// and assigment is deleted by default

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