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

constexpr double kEpsilon = std::numeric_limits<double>::epsilon();
constexpr double kPI = 3.141592654;
constexpr double kPI_mul2 = 6.283185307;
constexpr double kPI_div2 = 1.570796327;
constexpr double kPI_div4 = 0.785398163;
constexpr double kPI_inv = 0.318309886;

} // namespace anshub

#endif  // GM_CONST_H