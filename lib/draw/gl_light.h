// *************************************************************
// File:    gl_light.h
// Descr:   represents light
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef GL_LIGHT_H
#define GL_LIGHT_H

namespace anshub {

struct GlLight
{
  enum class Type
  {
    AMBIENT,
    DIRECION,
    SPOT
    
  }; // enum class Type

}; // struct GlLight 

}  // namespace anshub

#endif  // GL_LIGHT_H