// *************************************************************
// File:    mx_rotate_eul.h
// Descr:   row-ordered rotation matrix (YXZ sequence)
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef GC_MATRIX_ROTATE_EUL_H
#define GC_MATRIX_ROTATE_EUL_H

#include "lib/math/trig.h"
#include "lib/math/math.h"
#include "lib/math/vector.h"
#include "lib/math/matrix.h"

namespace anshub {

// Rotation matrix builded conventionally as sequence of YXZ rotations
// by multiplie sequentially each other exclude 0 degree angles 

struct MatrixRotateEul : public Matrix<4,4>
{
  MatrixRotateEul(float, float, float, const TrigTable&);
  MatrixRotateEul(const Vector&, const TrigTable&);

}; // struct MatrixRotate

// Constructs rotation matrixin regular way

inline MatrixRotateEul::MatrixRotateEul(
  float x, float y, float z, const TrigTable& t)
: Matrix(Type::IDENTITY)
{
  Matrix<4,4> res (Type::IDENTITY);
  
  if (math::FNotZero(y))     // see note #1 in the end of func
  {
    float tsin = t.Sin(y);
    float tcos = t.Cos(y);
    Matrix<4,4> yrot = {
      tcos,   0.0f,  -tsin,   0.0f,
      0.0f,   1.0f,   0.0f,   0.0f, 
      tsin,   0.0f,   tcos,   0.0f,
      0.0f,   0.0f,   0.0f,   1.0f 
    };
    res = matrix::Multiplie(res, yrot);    
  }
  if (math::FNotZero(x))
  {
    float tsin = t.Sin(x);
    float tcos = t.Cos(x);
    Matrix<4,4> xrot = {
      1.0f,   0.0f,   0.0f,   0.0f,
      0.0f,   tcos,   tsin,   0.0f, 
      0.0f,  -tsin,   tcos,   0.0f,
      0.0f,   0.0f,   0.0f,   1.0f 
    };
    res = matrix::Multiplie(res, xrot);
  }
  if (math::FNotZero(z))
  {
    float tsin = t.Sin(z);
    float tcos = t.Cos(z);
    Matrix<4,4> zrot = {
      tcos,   tsin,   0.0f,   0.0f,
     -tsin,   tcos,   0.0f,   0.0f, 
      0.0f,   0.0f,   1.0f,   0.0f,
      0.0f,   0.0f,   0.0f,   1.0f 
    };
    res = matrix::Multiplie(res, zrot);    
  }
  data_ = std::move(res.Data());
  
  // Note : if the angle == 0 then cos 0 = 1, sin 0 = 0, and we have
  // identity matrix
}

// Constructs rotation matrix by improved algorithm

inline MatrixRotateEul::MatrixRotateEul(const Vector& ang, const TrigTable& t)
: Matrix(Type::IDENTITY)
{
  // Precompute sin and cos of angles. Here I explicit doesn`t do check to
  // zero angle since calculations through table costs low

  float xs = t.Sin(ang.x);
  float xc = t.Cos(ang.x);
  float ys = t.Sin(ang.y);
  float yc = t.Cos(ang.y);
  float zs = t.Sin(ang.z);
  float zc = t.Cos(ang.z);

  // This matrix should be as result in first constructor, but without 
  // unnecessary multiplications. Also knowledge about configuration of this 
  // final matrix is necessary when we try to extract euler angles from
  // this matrix

  // Matrix<4,4> res = {
  //    yc*zc,  xs*ys*zc+xc*zs, xs*zs-xc*ys*zc, 0,
  //   -zs*yc,  xc*zc-xs*ys*zs, xc*ys*zs+zc*xs, 0,
  //    ys,    -xs*yc,          yc*xc,          0,
  //    0,      0,              0,              1
  // };

  Matrix<4,4> res = {
     yc*zc-xs*ys*zs,  yc*zs+xs*ys*zc, -ys*xc,   0,
    -zs*xc,           xc*zc,           xs,      0,
     ys*zc+zs*yc*xs,  ys*zs-zc*yc*xs,  yc*xc,   0,
     0,               0,               0,       1
  };

  // Here is present good explanation, but all matrixes
  // are not the same as here, thus needs to compute by hands
  // http://www.songho.ca/opengl/gl_anglestoaxes.html

  // Matrix<4,4> res = {
  //    yc*zc+ys*xs*zs,    xc*zs,    yc*xs*zs-ys*zc,   0,
  //    ys*xs*zc-yc*zs,    xc*zc,    ys*zs+yc*xs*zc,   0,
  //    ys*xc,            -xs,       yc*xc,            0,
  //    0,                 0,        0,                1
  // };

  data_ = std::move(res.Data());

}

}  // namespace anshub

#endif  // GC_MATRIX_ROTATE_EUL_H