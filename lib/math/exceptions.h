// *************************************************************
// File:    exceptions.h
// Descr:   contains exceptions class
// Author:  Novoselov Anton @ 2017
// URL:     https://github.com/ans-hub/geomath_lib
// *************************************************************

#ifndef SMPL_EXCEPT_H
#define SMPL_EXCEPT_H

#include <stdexcept>

namespace anshub {

struct MathExcept : std::logic_error
{
  MathExcept(const char* msg) : std::logic_error(msg) { }
};

} // namespace anshub

#endif  // SMPL_EXCEPT_H