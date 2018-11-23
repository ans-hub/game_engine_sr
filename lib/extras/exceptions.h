// *************************************************************
// File:    exceptions.h
// Descr:   contains exceptions class for extras lib
// Author:  Novoselov Anton @ 2017
// *************************************************************

#ifndef GC_EXTRAS_EXCEPT_H
#define GC_EXTRAS_EXCEPT_H

#include <stdexcept>

namespace anshub {

struct ExtrasExcept : std::logic_error
{
  ExtrasExcept(const char* msg) : std::logic_error(msg) { }

}; // struct ExtrasExcept

} // namespace anshub

#endif  // GC_EXTRAS_EXCEPT_H