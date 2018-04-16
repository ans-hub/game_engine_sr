// *************************************************************
// File:    gl_enums.h
// Descr:   enums used in draw lib
// Author:  Novoselov Anton @ 2017-2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef GC_DRAW_ENUMS_H
#define GC_DRAW_ENUMS_H

namespace anshub {

// Camera types

namespace CamType
{
  // For regular purposes

  enum Types
  {
    EULER,
    UVN,
    FOLLOW
    
  }; // enum Types

  // For templated functions
  
  enum class Euler { type };
  enum class Uvn { type };
  enum class Follow { type };

} // namespace CamType

// Used to define shading type of surface or object

enum class Shading
{
  NONE      = 0,
  CONST     = 1 << 1,
  FLAT      = 1 << 2,
  PHONG     = 1 << 3,
  GOURANG   = 1 << 4,       // I have left this error to compatibility
  GOURAUD   = 1 << 4

}; // enum class Shading

// Used to define which texturing we would use

enum class Texturing
{
  NONE      = 0,
  AFFINE    = 1 << 1,
  PERSP     = 1 << 2

}; // enum class Texturing

// Used to define which coordinates currently used in object

enum class Coords
{
  LOCAL,    // local coordinates
  TRANS     // transformed coordinates

}; // enum class Coords

// Represent Axis names

enum class Axis
{
  X         = 1 << 1,
  Y         = 1 << 2,
  XY        = 3,
  Z         = 1 << 3,
  XZ        = 5,
  YZ        = 6,
  XYZ       = 7

}; // enum class Axis

} // namespace anshub

#endif  // GC_DRAW_ENUMS_H