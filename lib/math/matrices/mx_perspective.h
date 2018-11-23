// *************************************************************
// File:    matrix_persp.h
// Descr:   row-based perspective matrix
// Author:  Novoselov Anton @ 2017
// *************************************************************

#ifndef GM_MATRIX_PERSP_H
#define GM_MATRIX_PERSP_H

#include "../matrix.h"
#include "../vector.h"

namespace anshub {

struct MatrixPerspective : public Matrix<4,4>
{
  MatrixPerspective(float, float);

}; // struct MatrixPerspective

inline MatrixPerspective::MatrixPerspective(float dov, float ar)
  : Matrix<4,4>
  ({
    dov,  0.0f,   0.0f,   0.0f,
    0.0f, dov*ar, 0.0f,   0.0f,
    0.0f, 0.0f,   1.0f,   1.0f,
    0.0f, 0.0f,   1.0f,   0.0f
  })
{ }

}  // namespace anshub

#endif  // GM_MATRIX_PERSP_H

// Note : we may interpet perspective matrix as we wish, take another
// form, etc. I.e., mx 1 0 0 0  0 1 0 0  0 0 1 1/d  0 0 0 0
// do the same