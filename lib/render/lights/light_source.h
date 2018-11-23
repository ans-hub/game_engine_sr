// *************************************************************
// File:    light_source.h
// Descr:   represents point light source
// Author:  Novoselov Anton @ 2017
// *************************************************************

#ifndef GL_LIGHT_SOURCE_H
#define GL_LIGHT_SOURCE_H

#include "lib/math/math.h"
#include "lib/math/trig.h"

#include "lib/render/gl_aliases.h"
#include "lib/render/fx_colors.h"
#include "lib/render/cameras/gl_camera.h"

namespace anshub {

//****************************************************************************
// Represents abstract light source
//****************************************************************************

template<class IlluminateArgs>
struct LightSource
{
  IlluminateArgs args_;

  LightSource(const FColor& color, float intense);
  virtual ~LightSource() =0;
  
  virtual void   Reset() =0;
  virtual void   World2Camera(const GlCamera&, cTrigTable&) =0;
  virtual FColor Illuminate() const =0;
  
  void SetColor(cFColor& c) { color_ = c; }
  void SetIntense(float i) { intense_ = i; }

protected:
  FColor     color_;
  float      intense_;

}; // struct LightSource

//****************************************************************************
// Inline implementation
//****************************************************************************

template<class IlluminateArgs>
LightSource<IlluminateArgs>::LightSource(cFColor& color, float intense)
  : args_{}
  , color_{color}
  , intense_{intense}
{
  math::Clamp(intense_, 0.0f, 1.0f);  
}

template<class IlluminateArgs>
LightSource<IlluminateArgs>::~LightSource()
{ }

}  // namespace anshub

#endif  // GL_LIGHT_SOURCE_H