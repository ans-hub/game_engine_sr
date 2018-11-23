// *************************************************************
// File:    matrix.h
// Descr:   represents matrix
// Author:  Novoselov Anton @ 2017
// *************************************************************

// TODO: return std::array instead of std::vector

#ifndef GM_MATRIX_H
#define GM_MATRIX_H

#include <array>
#include <algorithm>
#include <cmath>
#include <initializer_list>

#include "trig.h"
#include "vector.h"

namespace anshub {

template<std::size_t Row, std::size_t Col>
class Matrix
{
public:
  
  using Container = std::array<float, Row * Col>;

  enum Type { ZERO, IDENTITY }; // used to constructs zero or identity matrix

  Matrix();                                     // for zero matrix creating
  explicit Matrix(Type);                        // for identity matrix creating
  Matrix(const std::initializer_list<float>&);  // for custom matrix creating
  Matrix(const Matrix&) =default;
  Matrix& operator=(const Matrix&) =default;
  Matrix(Matrix&&) =default;
  Matrix& operator=(Matrix&&) =default;
  virtual ~Matrix() noexcept { }

  std::size_t Rows() const { return r_; }
  std::size_t Cols() const { return c_; }
  std::size_t Size() const { return data_.count(); }
  Container&  Data() { return data_; }
  const Container& Data() const { return data_; }

  Vector GetRow(std::size_t row) const;
  void SetRow(std::size_t row, const Vector&);

  // Non standart behaviour operators

  Vector operator*(const Vector&);
  Vector operator%(Vector);
  Matrix& operator*(const Matrix&);
  Matrix& operator%(const Matrix&);

  // Standart behavior operators

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
  float operator()(int i, int k) const
  {
    return data_[i * c_ + k];
  }
  float& operator()(int i, int k)
  {
    return data_[i * c_ + k];
  }
  friend inline Matrix operator*(Matrix lhs, float scalar)
  {
    lhs *= scalar;
    return lhs;
  }
  friend inline Matrix operator/(Matrix lhs, float scalar)
  {
    lhs /= scalar;
    return lhs;
  }
  friend inline Matrix operator-(Matrix lhs, const Matrix& rhs)
  {
    lhs -= rhs;
    return lhs;
  }
  friend inline Matrix operator+(Matrix lhs, const Matrix& rhs)
  {
    lhs += rhs;
    return lhs;
  }

protected:
  Container   data_;
  std::size_t r_;
  std::size_t c_;

}; // struct Matrix

//****************************************************************************
// DECLARATION: HELPER FUNCTIONS FOR MATRIX CLASS
//****************************************************************************

namespace matrix {

  // Special simple functions for frequently meets square matrices

  float   Determinant(const Matrix<2,2>&);
  float   Determinant(const Matrix<3,3>&);
  bool    Inverse(const Matrix<2,2>&, Matrix<2,2>&);
  bool    Inverse(const Matrix<3,3>&, Matrix<3,3>&);
  Vector  Multiplie(const Vector&, const Matrix<3,3>&);   // 2d+1 x mx<3,3>
  Vector  Multiplie(const Vector&, const Matrix<4,4>&);   // 3d+1 x mx<4,4>

  // Helper functions to transform matrices

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

  template<std::size_t R>
  void MakeRotateAxis(Matrix<R,R>&, float angle);

  Matrix<4,4> MakeRotateX(float angle);
  Matrix<4,4> MakeRotateY(float angle);
  Matrix<4,4> MakeRotateZ(float angle);

  // Other helpers

  template<std::size_t R, std::size_t C> 
  std::ostream& operator<<(std::ostream&, const Matrix<R,C>&);

} // namespace matrix

//****************************************************************************
// MATRIX SPECIALIZATION OF MOST COMMON MATRICES FUNCTIONS
//****************************************************************************

// Returns vector from matrix row

template<>
inline Vector Matrix<4,4>::GetRow(std::size_t r) const
{
  if (r >= r_)
    throw MathExcept("<Matrix<4,4>::operator() - row count invalid");
  return Vector(
    data_[r*c_], data_[r*c_+1], data_[r*c_+2], data_[r*c_+3]);
}

// Sets row of matrix by vector

template<>
inline void Matrix<4,4>::SetRow(std::size_t row, const Vector& v)
{
  data_[row*c_] = v.x;
  data_[row*c_+1] = v.y;
  data_[row*c_+2] = v.z;
  data_[row*c_+3] = v.w;
}

// Rotate and translate vector by matrix (see note after this code section)

template<>
inline Vector Matrix<4,4>::operator*(const Vector& v)
{
  Vector res {};
  res.x = data_[0] * v.x + data_[4] * v.y + data_[8] * v.z + data_[12] * v.w;
  res.y = data_[1] * v.x + data_[5] * v.y + data_[9] * v.z + data_[13] * v.w;
  res.z = data_[2] * v.x + data_[6] * v.y + data_[10] * v.z + data_[14] * v.w;
  res.w = 1.f;
  return res;
}

// Rotate without translate vector by matrix (see note after this code section)

template<>
inline Vector Matrix<4,4>::operator%(Vector v)
{
  v.w = 0.f;
  return *this * v;
}

// Note: w multiplication imitates multiplication implemented in hardware.
// Here I multiplie it just for clear educational reasons. In real life w == 0
// to not apply translation

//****************************************************************************
// IMPLEMENTATION: MATRIX MEMBER FUNCTIONS
//****************************************************************************

// Default constructor of zero matrix

template<std::size_t R, std::size_t C>
Matrix<R,C>::Matrix()
  : data_()
  , r_{R}
  , c_{C}
{ }

// Constructs the identity matrix if identity flag is on. Otherwise zero mx/
// With compile time check of squareness

template<std::size_t R, std::size_t C>
Matrix<R,C>::Matrix(Type t)
  : data_()
  , r_{R}
  , c_{C}
{
  if (t == Type::IDENTITY)
    matrix::MakeIdentityFromZero(*this);
}

// Constructs the matrix with initializer list

template<std::size_t R, std::size_t C>
Matrix<R,C>::Matrix(const std::initializer_list<float>& list)
  : data_{}
  , r_{R}
  , c_{C}
{
  if (data_.size() != list.size())
    throw MathExcept("Matrix::Matrix(): size and values are not same");
  int i {0};
  for (auto& item : list)
    data_[i++] = item;
}

// Multiplie matrix

template<std::size_t R, std::size_t C>   
Matrix<R,C>& Matrix<R,C>::operator*(const Matrix<R,C>& rhs)
{
  *this = matrix::Multiplie(*this, rhs);  
  return *this;
}

// Multiplie 4x4 in regular way but translation row is zero

template<std::size_t R, std::size_t C>   
Matrix<R,C>& Matrix<R,C>::operator%(const Matrix<R,C>& rhs)
{
  *this = matrix::Multiplie(*this, rhs);
  this->SetRow(3, Vector{0.f, 0.f, 0.f, 1.f});
  return *this;
}

//****************************************************************************
// IMPLEMENTATION: MATRICES HELPERS NON-MEMBER FUNCTIONS
//****************************************************************************

// Multiplie matrices (naive)

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
  mx = Matrix<R,R>();
  for (std::size_t i = 0; i < mx.Cols(); ++i)
    mx(i,i) = 1;
}

// Makes identity matrix (naive)

template<std::size_t R> 
void matrix::MakeIdentityFromZero(Matrix<R,R>& mx)
{
  // todo: add assert matrix is zero
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
