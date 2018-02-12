// *************************************************************
// File:    matrix_rotate_uvn.h
// Descr:   rotation matrix based on uvn vectors
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef GC_MATRIX_ROTATE_UVN_H
#define GC_MATRIX_ROTATE_UVN_H

#include "trig.h"
#include "math.h"
#include "vector.h"
#include "matrix.h"

namespace anshub {

// Rotation matrix builded from uvn vectors which are new basis 

struct MatrixRotateUvn : public Matrix<4,4>
{
  MatrixRotateUvn(cVector&, cVector&, cVector&);

}; // struct MatrixRotateUvn

// Constructs uvn rotation matrix

inline MatrixRotateUvn::MatrixRotateUvn(cVector& u, cVector& v, cVector& n)
: Matrix(Type::IDENTITY)
{
  Matrix<4,4> res = {
    u.x,  v.x,  n.x,  0.0f,
    u.y,  v.y,  n.y,  0.0f,
    u.z,  v.z,  n.z,  0.0f,
    0.0f, 0.0f, 0.0f, 1.0f,
  };

  data_ = std::move(res.Data());
}

}  // namespace anshub

#endif  // GC_MATRIX_ROTATE_EUL_H