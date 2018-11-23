// *************************************************************
// File:    matrix_rotate_uvn.h
// Descr:   preinverted rotation matrix based on uvn vectors
// Author:  Novoselov Anton @ 2017
// *************************************************************

#ifndef GC_MATRIX_ROTATE_UVN_H
#define GC_MATRIX_ROTATE_UVN_H

#include "../trig.h"
#include "../math.h"
#include "../vector.h"
#include "../matrix.h"

namespace anshub {

// Inverted rotation matrix builded from uvn vectors which are new
// basis. Already inverted matrix since directional vectors are in 
// columns not in rows as usually. Therefore any multiplication
// of vectors with this matrix would be interpreted as rotate
// back (as it dot product of p with each of axises therefore
// p' will have local coordinates of the uvn basis) 

struct MatrixRotateUvn : public Matrix<4,4>
{
  MatrixRotateUvn(cVector&, cVector&, cVector&);

}; // struct MatrixRotateUvn

// Constructs uvn rotation matrix

inline MatrixRotateUvn::MatrixRotateUvn(cVector& u, cVector& v, cVector& n)
  : Matrix<4,4>
  ({
    u.x,  v.x,  n.x,  0.0f,
    u.y,  v.y,  n.y,  0.0f,
    u.z,  v.z,  n.z,  0.0f,
    0.0f, 0.0f, 0.0f, 1.0f,
  })
{ }

}  // namespace anshub

#endif  // GC_MATRIX_ROTATE_UVN_H