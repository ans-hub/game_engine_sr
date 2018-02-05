// *************************************************************
// File:    exceptions.h
// Descr:   contains exceptions class for draw lib
// Author:  Novoselov Anton @ 2017-2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef GC_DRAW_EXCEPT_H
#define GC_DRAW_EXCEPT_H

#include <stdexcept>

namespace anshub {

struct DrawExcept : std::logic_error
{
  DrawExcept(const char* msg) : std::logic_error(msg) { }

}; // struct DrawExcept

} // namespace anshub

#endif  // GC_DRAW_EXCEPT_H