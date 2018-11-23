// *************************************************************
// File:    matrix_trans.h
// Descr:   row-based translate matrix
// Author:  Novoselov Anton @ 2017
// *************************************************************

#ifndef GC_MATRIX_TRANS_H
#define GC_MATRIX_TRANS_H

#include "../matrix.h"
#include "../vector.h"

namespace anshub {

struct MatrixTranslate : public Matrix<4,4>
{
  MatrixTranslate(const Vector&);
  MatrixTranslate(float, float, float);

}; // struct MatrixTranslate

inline MatrixTranslate::MatrixTranslate(const Vector& dt)
: Matrix
    ({
      1.0f, 0.0f, 0.0f, 0.0f,
      0.0f, 1.0f, 0.0f, 0.0f,
      0.0f, 0.0f, 1.0f, 0.0f,
      dt.x, dt.y, dt.z, 1.0f
    })
{ }

inline MatrixTranslate::MatrixTranslate(float x, float y, float z)
: Matrix
    ({
      1.0f, 0.0f, 0.0f, 0.0f,
      0.0f, 1.0f, 0.0f, 0.0f,
      0.0f, 0.0f, 1.0f, 0.0f,
      x,    y,    z,    1.0f
    })
{ }

}  // namespace anshub

#endif  // GC_MATRIX_TRANS_H