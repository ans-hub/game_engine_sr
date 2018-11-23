// *************************************************************
// File:    exceptions.h
// Descr:   contains exceptions class for draw lib
// Author:  Novoselov Anton @ 2017
// *************************************************************

#ifndef GC_DRAW_EXCEPT_H
#define GC_DRAW_EXCEPT_H

#include <stdexcept>

namespace anshub {

//***************************************************************************
// Exceptions struct used in render library
//***************************************************************************

struct RenderExcept : std::logic_error
{
  RenderExcept(const char* msg) : std::logic_error(msg) { }

}; // struct RenderExcept

}  // namespace anshub

#endif  // GC_DRAW_EXCEPT_H