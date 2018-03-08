// *************************************************************
// File:    fx_colors.h
// Descr:   helpers color functions 
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef GL_FXCOLORS_H
#define GL_FXCOLORS_H

#include <iostream>
#include <cmath>

#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "gl_aliases.h"

namespace anshub {

//***************************************************************************
// Interface to Color struct
//***************************************************************************

template<class T = uint>
struct Color
{
  Color();
  Color(T r, T g, T b);
  Color(T r, T g, T b, T a);
  explicit Color(unsigned int);

  uint  GetARGB() const;
  void  Modulate(const Color&);
  void  Clamp();

  Color& operator/=(T scalar);
  Color& operator/=(int scalar);
  Color& operator*=(T scalar);
  Color& operator*=(int scalar);
  Color& operator*=(const Color& rhs);
  Color& operator-=(const Color& rhs);
  Color& operator+=(const Color& rhs);
  template<class U> friend Color<U> operator/ (Color<U>, U scalar);
  template<class U> friend Color<U> operator/ (Color<U>, int scalar);
  template<class U> friend Color<U> operator*(Color<U>, U scalar);
  template<class U> friend Color<U> operator*(Color<U>, int scalar);
  template<class U> friend Color<U> operator*(Color<U>, const Color<U>&);
  template<class U> friend Color<U> operator-(Color<U>, const Color<U>&);
  template<class U> friend Color<U> operator+(Color<U>, const Color<U>&);

  T r_;
  T g_;
  T b_;
  T a_;

}; // struct Color

//***************************************************************************
// Interface to Color helper functions
//***************************************************************************

namespace color {

  // Colors constants

  constexpr uint White {0xffffffff};
  constexpr uint Blue  {0xff000000};
  constexpr uint Black {0x00000000};

  // Helpers functions

  int   MakeARGB(uchar a, uchar r, uchar g, uchar b);
  void  SplitARGB(int color, uchar& b, uchar& g, uchar& r, uchar& a);
  int   IncreaseBrightness(int color, float k);

  // Output functions

  template<class T>
  std::ostream& operator<<(std::ostream& oss, const Color<T>& c);

} // namespace color

//***************************************************************************
// Implementation of inline member functions
//***************************************************************************

// Default Color constructor

template<class T>
inline Color<T>::Color() 
  : r_{0}
  , g_{0}
  , b_{0}
  , a_{255} { }

// Constructs Color with given color components (using alpha)

template<class T>
inline Color<T>::Color(T cr, T cg, T cb, T ca)
  : r_{static_cast<T>(cr)}
  , g_{static_cast<T>(cg)}
  , b_{static_cast<T>(cb)}
  , a_{static_cast<T>(ca)} { }

// Constructs Color with given color components (using alpha)

template<class T>
inline Color<T>::Color(T cr, T cg, T cb)
  : r_{static_cast<T>(cr)}
  , g_{static_cast<T>(cg)}
  , b_{static_cast<T>(cb)}
  , a_{255} { }

// Constructs Color with unsigned represent of color

template<class T>
inline Color<T>::Color(unsigned int c)
  : r_{static_cast<T>((c >> 8) & 0xff)}
  , g_{static_cast<T>((c >> 16) & 0xff)}
  , b_{static_cast<T>((c >> 24) & 0xff)}
  , a_{static_cast<T>(c & 0xff)} { }

// Returns color in uint representation

template<class T>
inline uint Color<T>::GetARGB() const
{
  return ((int)b_ << 24) | ((int)g_ << 16) | ((int)r_ << 8) | 255;
}

// Modulates 2 colors

template<class T>
inline void Color<T>::Modulate(const Color<T>& rhs)
{ 
  *this *= rhs;
  *this /= 256;
}

// Clamps color components after addition

template<class T>
inline void Color<T>::Clamp()
{
  r_ = std::min(255.0f, r_);
  g_ = std::min(255.0f, g_);
  b_ = std::min(255.0f, b_);
  a_ = 255.0f;
}

// Other useful member functions implementation

template<class T>
inline Color<T>& Color<T>::operator/=(T scalar)
{
  this->r_ /= scalar;
  this->g_ /= scalar;
  this->b_ /= scalar;
  this->a_ /= scalar;
  return *this;
}

template<class T>
inline Color<T>& Color<T>::operator/=(int scalar)
{
  this->r_ /= scalar;
  this->g_ /= scalar;
  this->b_ /= scalar;
  this->a_ /= scalar;
  return *this;
}

template<class T>
inline Color<T>& Color<T>::operator*=(T scalar)
{
  this->r_ *= scalar;
  this->g_ *= scalar;
  this->b_ *= scalar;
  this->a_ *= scalar;
  return *this;
}

template<class T>
inline Color<T>& Color<T>::operator*=(int scalar)
{
  this->r_ *= scalar;
  this->g_ *= scalar;
  this->b_ *= scalar;
  this->a_ *= scalar;
  return *this;
}

template<class T>
inline Color<T>& Color<T>::operator*=(const Color& rhs)
{
  this->r_ *= rhs.r_;
  this->g_ *= rhs.g_;
  this->b_ *= rhs.b_;
  this->a_ *= rhs.a_;
  return *this;
}

template<class T>
inline Color<T>& Color<T>::operator-=(const Color& rhs)
{
  this->r_ -= rhs.r_;
  this->g_ -= rhs.g_;
  this->b_ -= rhs.b_;
  this->a_ -= rhs.a_;
  return *this;
}

template<class T>
inline Color<T>& Color<T>::operator+=(const Color& rhs)
{
  this->r_ += rhs.r_;
  this->g_ += rhs.g_;
  this->b_ += rhs.b_;
  this->a_ += rhs.a_;
  return *this;
}

// Friend non-member functions implementation

template<class U>
inline Color<U> operator/(Color<U> lhs, U scalar)
{
  lhs /= scalar;
  return lhs;
}

template<class U>
inline Color<U> operator/(Color<U> lhs, int scalar)
{
  lhs /= scalar;
  return lhs;
}

template<class U>
inline Color<U> operator*(Color<U> lhs, U scalar)
{
  lhs *= scalar;
  return lhs;
}

template<class U>
inline Color<U> operator*(Color<U> lhs, int scalar)
{
  lhs *= scalar;
  return lhs;
}

template<class U>
inline Color<U> operator*(Color<U> lhs, const Color<U>& rhs)
{
  lhs *= rhs;
  return lhs;
}

template<class U>
inline Color<U> operator-(Color<U> lhs, const Color<U>& rhs)
{
  lhs -= rhs;
  return lhs;
}

template<class U>
inline Color<U> operator+(Color<U> lhs, const Color<U>& rhs)
{
  lhs += rhs;
  return lhs;
}

//***************************************************************************
// IMPLEMENTATION of inline helper functions
//***************************************************************************

// Split ARGB byte-ordered little-endian into r,g,b,a components

inline void color::SplitARGB(int color, uchar& b, uchar& g, uchar& r, uchar& a)
{
  b = (color >> 24) & 0xff;
  g = (color >> 16) & 0xff;
  r = (color >> 8)  & 0xff;
  a = color & 0xff;
}

// Returns color in word-order ARGB format (little-endian) where the most
// signification byte is b, and less is a

inline int color::MakeARGB(uchar a, uchar r, uchar g, uchar b)
{
  return (b << 24) | (g << 16) | (r << 8) | a;
}

// Prints on output stream color information

template<class T>
std::ostream& operator<<(std::ostream& oss, const Color<T>& c)
{
  oss << c.r_ << ';' << c.g_ << ';' << c.b_ << ';' << c.a_;
  return oss;
}

} // namespace anshub

#endif  // GL_FXCOLORS_H