// *************************************************************
// File:    matrix_persp.h
// Descr:   perspective matrix
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/geomath_lib
// *************************************************************

#ifndef GM_MATRIX_PERSP_H
#define GM_MATRIX_PERSP_H

#include "../matrix.h"
#include "../vector.h"

namespace anshub {

// Struct - matrix of perspective projection

struct MatrixPerspective : public Matrix<4,4>
{
  MatrixPerspective(float, float);

}; // struct MatrixTranslate

inline MatrixPerspective::MatrixPerspective(float dov, float ar)
: Matrix
  ({
    dov,  0.0f,   0.0f,   0.0f,
    0.0f, dov*ar, 0.0f,   0.0f,
    0.0f, 0.0f,   10.0f,   1.0f,    // todo: why and what the magic consts?
    0.0f, 0.0f,   -10.1f,   0.0f
  })
{ }

}  // namespace anshub

#endif  // GM_MATRIX_PERSP_H

// Note #1 :
//  dov - distance of view
//  ar  - aspect ratio

// Note #2 : as I understood we may interpet perspective matrix as we wish,
// take another form, etc. I.e., mx 1 0 0 0  0 1 0 0  0 0 1 1/d  0 0 0 0
// do the same