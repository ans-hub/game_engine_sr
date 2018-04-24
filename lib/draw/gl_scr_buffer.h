// *************************************************************
// File:    gl_scr_buffer.h
// Descr:   emulates screen buffer for rendering
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef GL_SCR_BUFFER_H
#define GL_SCR_BUFFER_H

#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES     // to get prototype functions from glext.h
#endif

#include <vector>
#include <string.h>             // memset
#include <GL/gl.h>
#include <GL/glext.h>

#include "lib/draw/gl_aliases.h"

namespace anshub {

//****************************************************************************
// Screen buffer emulator class
//****************************************************************************

struct ScrBuffer
{
  ScrBuffer(int w, int h, int color);

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
  V_Uint ptr_;      // 32 bit color buffer

}; // class ScrBuffer

//****************************************************************************
// Implementation of inline member functions
//****************************************************************************

inline void ScrBuffer::Clear()
{
  memset(ptr_.data(), 0, w_*h_*sizeof(*ptr_.data())); 
  
  // ... forced to use memset instead std::fill after profiling
}

}  // namespace anshub

#endif  // GL_SCR_BUFFER_H

// Important note : 0,0 is the left-bottom corner