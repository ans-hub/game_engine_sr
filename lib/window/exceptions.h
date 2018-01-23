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
  {
    msg_ = (char*)malloc(strlen(msg) + strlen(std::strerror(num) + 1));
  }
    
  ~IOException() { if (msg_) free(msg_); }

  virtual const char* what() const throw() { return msg_; }

private:
  char* msg_;

}; // struct IOException

}  // namespace anshub

#endif  // IO_EXCEPTION_H