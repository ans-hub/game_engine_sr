// *************************************************************
// File:    matrix_scale.h
// Descr:   scale matrix
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef GC_MATRIX_SCALE_H
#define GC_MATRIX_SCALE_H

#include "../matrix.h"

namespace anshub {

struct MatrixScale : public Matrix<4,4>
{
  MatrixScale(float);
  MatrixScale(const Vector&);

}; // struct MatrixScale

inline MatrixScale::MatrixScale(float fact)
: Matrix
  ({
      fact, 0.0f, 0.0f, 0.0f,
      0.0f, fact, 0.0f, 0.0f,
      0.0f, 0.0f, fact, 0.0f,
      0.0f, 0.0f, 0.0f, 1.0f
  })
{ }

inline MatrixScale::MatrixScale(const Vector& v)
: Matrix
  ({
      v.x,  0.0f, 0.0f, 0.0f,
      0.0f, v.y,  0.0f, 0.0f,
      0.0f, 0.0f, v.z,  0.0f,
      0.0f, 0.0f, 0.0f, 1.0f
  })
{ }

}  // namespace anshub

#endif  // GC_MATRIX_SCALE_H