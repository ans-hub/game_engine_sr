// *************************************************************
// File:    enums.h
// Descr:   exceptions class for iogame_lib
// Author:  Novoselov Anton @ 2017
// URL:     https://github.com/ans-hub/iogame_lib
// *************************************************************

#ifndef IO_EXCEPTION_H
#define IO_EXCEPTION_H

#include <stdexcept>
#include <cerrno>
#include <cstring>

namespace anshub {

struct IOException : std::runtime_error
{
  IOException(const char* msg, int num)
    : std::runtime_error(msg)
    , msg_{nullptr}
    , res_{ asprintf(&msg_, "%s - %s", msg, std::strerror(num)) } { }
    
  ~IOException() { if (res_ && msg_) free(msg_); }

  virtual const char* what() const throw() { return msg_; }

private:
  char* msg_;
  int   res_;   // see note #1

}; // struct IOException

}  // namespace anshub

#endif  // IO_EXCEPTION_H

// Note #1 : asprintf join c-strings on-the-fly (internally allocate memory
//  for it). Some compilers are send warnings about unused result of asprintf