// *************************************************************
// File:    matrix.h
// Descr:   represents matrix
// Author:  Novoselov Anton @ 2017-2018
// URL:     https://github.com/ans-hub/geomath_lib
// *************************************************************

#ifndef GM_MATRIX_H
#define GM_MATRIX_H

#include <vector>
#include <algorithm>
#include <initializer_list>

#include "vector.h"

namespace anshub {

//****************************************************************************
// MATRIX CLASS DEFINITION
//****************************************************************************

template<std::size_t Row, std::size_t Col>
class Matrix
{
public:
  
  using Container = std::vector<float>;

  enum Type { ZERO, IDENTITY }; // used to constructs zero or identity matrix

  Matrix();                               // for zero matrix creating
  explicit Matrix(Type);                  // for identity matrix creating
  Matrix(std::initializer_list<float>);   // for custom matrix creating
  virtual ~Matrix() { }

  std::size_t Rows() const { return r_; }
  std::size_t Cols() const { return c_; }
  std::size_t Size() const { return size_; }
  Container   Data() const { return data_; }

  Matrix& operator*=(float s)
  {
    std::for_each (
      data_.begin(), data_.end(), [&s](float& e){ e *= s; }
    );
    return *this;
  }
  Matrix& operator/=(float s)
  {
    if (math::Fzero(s))
      throw MathExcept("Matrix::operator/: zero divide");      
    std::for_each (
      data_.begin(), data_.end(), [&s](float& e){ e /= s; }
    );
    return *this;
  }
  Matrix& operator+=(const Matrix& rhs)
  {
    std::transform
    (
      data_.begin(), data_.end(), rhs.data_.begin(), data_.begin(),
      std::plus<float>()
    );
    return *this;
  }
  Matrix& operator-=(const Matrix& rhs)
  {
    std::transform
    (
      data_.begin(), data_.end(), rhs.data_.begin(), data_.begin(),
      std::minus<float>()
    ); 
    return *this;
  }
  float operator()(int i, int k) const {
    return data_[i * c_ + k];
  }
  float& operator()(int i, int k)
  {
    return data_[i * c_ + k];
  }
  friend inline Matrix operator*(Matrix lhs, float scalar) {
    lhs *= scalar;
    return lhs;
  }
  friend inline Matrix operator/(Matrix lhs, float scalar) {
    lhs /= scalar;
    return lhs;
  }
  friend inline Matrix operator-(Matrix lhs, const Matrix& rhs) {
    lhs -= rhs;
    return lhs;
  }
  friend inline Matrix operator+(Matrix lhs, const Matrix& rhs)
  {
    lhs += rhs;
    return lhs;
  }

protected:
  std::size_t r_;
  std::size_t c_;
  std::size_t size_;
  Container   data_;

}; // struct Matrix

//****************************************************************************
// DEFINITION: HELPER FUNCTIONS FOR MATRIX CLASS
//****************************************************************************

namespace matrix {

  // Special simple functions for frequently meets square matrixes

  float   Determinant(const Matrix<2,2>&);
  float   Determinant(const Matrix<3,3>&);
  bool    Inverse(const Matrix<2,2>&, Matrix<2,2>&);
  bool    Inverse(const Matrix<3,3>&, Matrix<3,3>&);
  Vector  Multiplie(const Vector&, const Matrix<3,3>&);   // 2d+1 x mx<3,3>
  Vector  Multiplie(const Vector&, const Matrix<4,4>&);   // 3d+1 x mx<4,4>
  Vector  Multiplie(const Point&, const Matrix<4,4>&);    // 3d+1 x mx<4,4>

  // Helper functions to transform matrixes

  template<std::size_t R1, std::size_t R2, std::size_t C>   
  Matrix<R1,R2> Multiplie(const Matrix<R1,C>&, const Matrix<C,R2>&);

  template<std::size_t R, std::size_t C> 
  Matrix<C,R> Transpose(const Matrix<R,C>&);  

  template<std::size_t R> 
  Matrix<R,R> MakeIdentity();

  template<std::size_t R>
  void MakeIdentity(Matrix<R,R>&);

  template<std::size_t R>
  void MakeIdentityFromZero(Matrix<R,R>&);

  // Other helpers

  template<std::size_t R, std::size_t C> 
  std::ostream& operator<<(std::ostream&, const Matrix<R,C>&);

} // namespace matrix

//****************************************************************************
// IMPLEMENTATION: MATRIX MEMBER FUNCTIONS
//****************************************************************************

// Default constructor of zero matrix

template<std::size_t R, std::size_t C>
Matrix<R,C>::Matrix()
  : r_{R}
  , c_{C}
  , size_{r_ * c_}
  , data_(size_)
{ }

// Constructs the identity matrix if identity flag is on. Otherwise zero mx/
// With compile time check of squareness

template<std::size_t R, std::size_t C>
Matrix<R,C>::Matrix(Type t)
  : r_{R}
  , c_{C}
  , size_{r_ * c_}
  , data_(size_)
{
  if (t == Type::IDENTITY)
    matrix::MakeIdentityFromZero(*this);
}

// Constructs the matrix with initializer list

template<std::size_t R, std::size_t C>
Matrix<R,C>::Matrix(std::initializer_list<float> list)
  : r_{R}
  , c_{C}
  , size_{r_ * c_}
  , data_{list}
{
  if (size_ != list.size())
    throw MathExcept("Matrix::Matrix(): size and values are not same");
}

//****************************************************************************
// IMPLEMENTATION: MATRIX HELPERS NON-MEMBER FUNCTIONS
//****************************************************************************

// Multiplie matrixes (naive)

template<std::size_t R1, std::size_t R2, std::size_t C>   
Matrix<R1,R2> matrix::Multiplie(const Matrix<R1,C>& mx1, const Matrix<C,R2>& mx2)
{
  Matrix<R1,R2> res;

  for(std::size_t row = 0; row < res.Rows(); ++row)
    for(std::size_t col = 0; col < res.Cols(); ++col)
      for(std::size_t inner = 0; inner < mx1.Cols(); ++inner)
        res(row,col) += mx1(row,inner) * mx2(inner,col);  //mx1_row * mx2_col

  return res;
}

// Returns transposed matrix (naive)

template<std::size_t R, std::size_t C> 
Matrix<C,R> matrix::Transpose(const Matrix<R,C>& mx)
{
  Matrix<C,R> res;
  for (std::size_t i = 0; i < mx.Rows(); ++i)
  {
    for (std::size_t k = 0; k < mx.Cols(); ++k)
      res(k,i) = mx(i,k);
  }
  return res;
}

// Returns identity matrix (naive)

template<std::size_t R> 
Matrix<R,R> matrix::MakeIdentity()
{
  Matrix<R,R> res;
  for (std::size_t i = 0; i < res.Cols(); ++i)
    res(i,i) = 1;
  return res;
}

// Makes identity matrix (naive)

template<std::size_t R> 
void matrix::MakeIdentity(Matrix<R,R>& mx)
{
  mx = Matrix<R,R>(); // clears matrix to zeroes
  for (std::size_t i = 0; i < mx.Cols(); ++i)
    mx(i,i) = 1;
}

// Makes identity matrix (naive). Supposed tha mx is zero mx

template<std::size_t R> 
void matrix::MakeIdentityFromZero(Matrix<R,R>& mx)
{
  for (std::size_t i = 0; i < mx.Cols(); ++i)
    mx(i,i) = 1;
}

// Prints matrix content

template<std::size_t R, std::size_t C>  
std::ostream& matrix::operator<<(std::ostream& oss, const Matrix<R,C>& mx)
{
  for (std::size_t i = 0; i < mx.Rows(); ++i)
  {
    for (std::size_t k = 0; k < mx.Cols(); ++k)
      oss << mx(i,k) << ' ';

    if (i+1 < mx.Rows())
      oss << '\n';
  }
  return oss;
}

} // namespace anshub

#endif  // GM_MATRIX_H

// Notes: to create identity mx: Matrix<3,3> news (Matrix<3,3>::IDENTITY);

// Todo : make implementation based on std::array ???