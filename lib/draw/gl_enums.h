// *************************************************************
// File:    gl_enums.h
// Descr:   enums used in draw lib
// Author:  Novoselov Anton @ 2017-2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef GC_DRAW_ENUMS_H
#define GC_DRAW_ENUMS_H

#include <type_traits>

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

// Represents Axis names

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

// Represents auxilary flags for GlObject

enum class AuxFlags : int
{
  NONE      = 1,
  COLLIDED  = 1 << 1  

}; // enum class AuxFlags
 
// Overloaded operator & allows to use AuxFlags values in boolean expressions

inline AuxFlags operator&(AuxFlags lhs, AuxFlags rhs)
{
  using T = std::underlying_type_t <AuxFlags>;
  return static_cast<AuxFlags>(
    (static_cast<T>(lhs) & static_cast<T>(rhs)));
}

inline AuxFlags& operator &=(AuxFlags& lhs, AuxFlags rhs)
{
  using T = std::underlying_type_t <AuxFlags>;
  lhs = (AuxFlags)(static_cast<T>(lhs) & static_cast<T>(rhs));
  return lhs;
}


// The same as above but for logical OR

inline AuxFlags operator|(AuxFlags lhs, AuxFlags rhs)
{
  using T = std::underlying_type_t <AuxFlags>;
  return static_cast<AuxFlags>(
    (static_cast<T>(lhs) | static_cast<T>(rhs)));
}

inline AuxFlags& operator |=(AuxFlags& lhs, AuxFlags rhs)
{
  using T = std::underlying_type_t <AuxFlags>;
  lhs = (AuxFlags)(static_cast<T>(lhs) | static_cast<T>(rhs));
  return lhs;
}

} // namespace anshub

#endif  // GC_DRAW_ENUMS_H