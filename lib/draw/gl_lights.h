// *************************************************************
// File:    gl_lights.h
// Descr:   represents light
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef GL_LIGHTS_H
#define GL_LIGHTS_H

#include "fx_colors.h"
#include "gl_aliases.h"
#include "gl_object.h"
#include "gl_triangle.h"
#include "../math/vector.h"

namespace anshub {

struct LightAmbient
{
  LightAmbient(FColor& c, float i)
  : color_{c}
  , intense_{i}
  {
    math::Clamp(intense_, 0.0f, 1.0f);      
  }
  LightAmbient(FColor&& c, float i)
  : color_{c}
  , intense_{i}
  {
    math::Clamp(intense_, 0.0f, 1.0f);      
  }
  FColor  color_;
  float   intense_; 

}; // struct LightsAmbient

struct LightInfinite
{
  LightInfinite(cFColor& c, float i, cVector& dir)
  : color_{c}
  , intense_{i}
  , direction_{dir}
  { 
    direction_.Normalize();
    math::Clamp(intense_, 0.0f, 1.0f);  
  }
  LightInfinite(cFColor&& c, float i, cVector&& dir)
  : color_{c}
  , intense_{i}
  , direction_{dir}
  { 
    direction_.Normalize();
    math::Clamp(intense_, 0.0f, 1.0f);  
  }
  
  FColor  color_;
  float   intense_;
  Vector  direction_;

}; // struct LightInfinite


struct LightPoint
{
  LightPoint(cFColor& c, float i, cVector& pos)
  : color_{c}
  , intense_{i}
  , position_{pos}
  { 
    math::Clamp(intense_, 0.0f, 1.0f);  
  }
  LightPoint(cFColor&& c, float i, cVector&& pos)
  : color_{c}
  , intense_{i}
  , position_{pos}
  { 
    math::Clamp(intense_, 0.0f, 1.0f);  
  }
  
  FColor  color_;
  float   intense_;
  Vector  position_;
  float   ka;
  float   kb;
  float   kc;

}; // struct LightPoint

struct Lights
{
  LightsAmbient   ambient_;
  LightsInfinite  infinite_;
  LightsPoint     point_;
  // LightsSpot      spot_;

}; // struct Lights

namespace light {
  
  void Object(GlObject&, Lights&);
  void Objects(GlObjects&, Lights&);
  void ConstShading(Triangle&, FColors&);
  void AmbientShading(
    Triangle&, cFColor&, cFColor&, cFColor&, LightsAmbient&);
  void InfiniteFlatShading(
    Triangle&, cFColor&, cFColor&, cFColor&, LightsInfinite&);
  void PointFlatShading(
    Triangle&, cFColor&, cFColor&, cFColor&, LightsInfinite&);
  void InfiniteGourangShading(
    Triangle&, Vectors&, cFColor&, cFColor&, cFColor&, LightsInfinite&);
  
}

}  // namespace anshub

#endif  // GL_LIGHTS_H