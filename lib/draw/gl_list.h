// *************************************************************
// File:    gl_list.h
// Descr:   container for triangles
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef GC_GL_LIST_H
#define GC_GL_LIST_H

#include <vector>

#include "lib/draw/gl_triangle.h"

namespace anshub {

class RenderList
{
  RenderList();

  std::vector<Triangle*>  ptrs_;  // see note #1 after code
  std::vector<Triangle>   data_;

}; // struct RenderList

namespace tri_list {

  

}  // namespace tri_list

}  // namespace anshub

#endif  // GC_GL_LIST_H

// Note #1 : we just manipulate ptrs and don`t touch data_, however this
// way more economy