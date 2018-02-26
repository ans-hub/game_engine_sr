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

// Simple struct represents color

template<class T = uchar>
struct Color
{
  Color() 
    : r{}, g{}, b{}, a{255} { }
  explicit Color(unsigned int c)
    : r {static_cast<T>((c >> 8) & 0xff)}
    , g {static_cast<T>((c >> 16) & 0xff)}
    , b {static_cast<T>((c >> 24) & 0xff)}
    , a {static_cast<T>(c & 0xff)} { }
  Color(T cr, T cg, T cb)
    : r{cr}, g{cg}, b{cb}, a{255} { }
  
  T r;
  T g;
  T b;
  T a;
  
  uint GetARGB() const {
    return ((int)b << 24) | ((int)g << 16) | ((int)r << 8) | 255;
  }
  void Normalize() { 
    r = std::fmod(r, 256.0f); 
    g = std::fmod(g, 256.0f);
    b = std::fmod(b, 256.0f);
    a = 255.0f;
  }
  void Clamp() {
    r = std::min(255.0f, r);
    g = std::min(255.0f, g);
    b = std::min(255.0f, b);
    a = 255.0f;
  }
  
  Color& operator/=(T scalar) {
    this->r /= scalar;
    this->g /= scalar;
    this->b /= scalar;
    this->a /= scalar;
    return *this;
  }

  // todo : decide what to do with overflow

  Color& operator*=(T scalar) {
    this->r *= scalar;
    this->g *= scalar;
    this->b *= scalar;
    this->a *= scalar;
    return *this;
  }

  Color& operator*=(const Color& rhs) {
    this->r *= rhs.r;
    this->g *= rhs.g;
    this->b *= rhs.b;
    this->a *= rhs.a;
    return *this;
  }

  Color& operator-=(const Color& rhs) {
    this->r -= rhs.r;
    this->g -= rhs.g;
    this->b -= rhs.b;
    this->a -= rhs.a;
    return *this;
  }

  Color& operator+=(const Color& rhs) {
    this->r += rhs.r;
    this->g += rhs.g;
    this->b += rhs.b;
    this->a += rhs.a;
    return *this;
  }

  friend inline Color operator/(Color lhs, T scalar) {
    lhs /= scalar;
    return lhs;
  }

  friend inline Color operator*(Color lhs, T scalar) {
    lhs *= scalar;
    return lhs;
  }
  
  friend inline Color operator*(Color lhs, const Color& rhs) {
    lhs *= rhs;
    return lhs;
  }

  friend inline Color operator-(Color lhs, const Color& rhs) {
    lhs -= rhs;
    return lhs;
  }

  friend inline Color operator+(Color lhs, const Color& rhs) {
    lhs += rhs;
    return lhs;
  }

}; // struct Color

namespace color {

  // Const colors

  constexpr uint White {0xffffffff};
  constexpr uint Blue  {0xff000000};
  constexpr uint Black {0x00000000};

  // Helpers functions

  int   MakeARGB(byte a, byte r, byte g, byte b);
  void  SplitARGB(int color, byte& b, byte& g, byte& r, byte& a);
  int   IncreaseBrightness(int color, float k);

  template<class T>
  std::ostream& operator<<(std::ostream& oss, const Color<T>& c)
  {
    oss << c.r << ';' << c.g << ';' << c.b << ';' << c.a;
    return oss;
  }

} // namespace color

// Implementation of inline functions

// Returns color in word-order ARGB format (little-endian) where the most
// signification byte is b, and less is a

inline int color::MakeARGB(byte a, byte r, byte g, byte b)
{
  return (b << 24) | (g << 16) | (r << 8) | a;
}

} // namespace anshub

#endif  // GL_FXCOLORS_H