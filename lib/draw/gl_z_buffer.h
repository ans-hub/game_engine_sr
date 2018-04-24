// *************************************************************
// File:    gl_z_buffer.h
// Descr:   z-buffer struct
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef GL_Z_BUFFER_H
#define GL_Z_BUFFER_H

#include <vector>
#include <algorithm>

#include "gl_aliases.h"

namespace anshub {

//****************************************************************************
// ZBuffer structs used to implement 1/z buffer
//****************************************************************************

struct ZBuffer
{
  ZBuffer(int w, int h)
  : w_{w}
  , h_{h}
  , writed_{0}
  , data_(w_ * h_, 0.0f) { }

  void    Clear();
  void    Writed() { ++writed_; }
  int     GetWrited() const { return writed_; }
  int     Width() const { return w_; }
  int     Height() const { return h_; }
  float*  GetPointer() { return data_.data(); }
  float&  operator()(int x, int y) { return data_[y * w_ + x]; }
  const float& operator()(int x, int y) const { return data_[y * w_ + x]; }

private:
  int w_;
  int h_;
  int writed_;    // debug info how much pixels was writed during frame
  V_Float data_;

}; // struct ZBuffer

//****************************************************************************
// Inline implementation
//****************************************************************************

// Clears zbuffer and debug info

inline void ZBuffer::Clear()
{
  writed_ = 0;
  memset(data_.data(), 0.0f, w_*h_*sizeof(*data_.data()));
  
  // ... forced to use memset instead std::fill after profiling
}

}  // namespace anshub

#endif  // GL_Z_BUFFER_H