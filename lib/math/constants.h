// *************************************************************
// File:    constants.h
// Descr:   contains math constants
// Author:  Novoselov Anton @ 2017-2018
// URL:     https://github.com/ans-hub/geomath_lib
// *************************************************************

#ifndef GM_CONST_H
#define GM_CONST_H

#include <limits>

namespace anshub {

namespace math {

constexpr float kEpsilon = std::numeric_limits<float>::epsilon();
constexpr float kPI = 3.14159265358979323846;
constexpr float kPI_mul2 = 6.283185307;
constexpr float kPI_div2 = 1.570796327;
constexpr float kPI_div4 = 0.785398163;
constexpr float kPI_inv = 0.318309886;

} // namespace math

} // namespace anshub

#endif  // GM_CONST_H