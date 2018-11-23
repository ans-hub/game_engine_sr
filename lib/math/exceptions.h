// *************************************************************
// File:    exceptions.h
// Descr:   contains exceptions class for math lib
// Author:  Novoselov Anton @ 2017
// *************************************************************

#ifndef GM_EXCEPT_H
#define GM_EXCEPT_H

#include <stdexcept>

namespace anshub {

struct MathExcept : std::logic_error
{
  MathExcept(const char* msg) : std::logic_error(msg) { }

}; // struct MathExcept

} // namespace anshub

#endif  // GM_EXCEPT_H