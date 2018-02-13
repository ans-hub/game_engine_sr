// *************************************************************
// File:    gl_buffer.h
// Descr:   emulates buffer for manual rendering
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef GL_BUFFER_H
#define GL_BUFFER_H

#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES     // to get prototype functions from glext.h
#endif

#include <vector>
#include <string.h>             // memset
#include <GL/gl.h>
#include <GL/glext.h>

#include "gl_aliases.h"

namespace anshub {

class Buffer
{
public:
  Buffer(int, int, int);

  void  Clear();
  void  SendDataToFB();
  
  uint* GetPointer() { return ptr_.data(); }
  int   Width() const { return w_; }
  int   Height() const { return h_; }
  uint& operator[](std::size_t i) { return ptr_[i]; }
  const uint& operator[](std::size_t i) const { return ptr_[i]; }

private:  
  int w_;           // buffer width
  int h_;           // buffer height
  int clear_color_; // -
  GLenum format_;   // https://goo.gl/2A58hH
  GLenum type_;     // the same as above
  Vuint ptr_;       // 32 bit color buffer

}; // class Buffer

}  // namespace anshub

#endif  // GL_BUFFER_H