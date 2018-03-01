// *************************************************************
// File:    gl_enums.h
// Descr:   enums used in draw lib
// Author:  Novoselov Anton @ 2017-2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef GC_DRAW_ENUMS_H
#define GC_DRAW_ENUMS_H

namespace anshub {

// Used to define shading type of surface or object

enum class Shading
{
  CONST     = 1 << 1,
  FLAT      = 1 << 2,
  PHONG     = 1 << 3,
  GOURANG   = 1 << 4

}; // enum class Shading

// Used to define which coordinates currently used in object

enum class Coords
{
  LOCAL,    // local coordinates
  TRANS     // transformed coordinates
};

} // namespace anshub

#endif  // GC_DRAW_ENUMS_H