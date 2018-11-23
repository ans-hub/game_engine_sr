// *************************************************************
// File:    infinite.h
// Descr:   represents infinite light source
// Author:  Novoselov Anton @ 2017
// *************************************************************

#ifndef GL_LIGHT_INFINITE_H
#define GL_LIGHT_INFINITE_H

#include "light_source.h"

#include "lib/render/gl_aliases.h"
#include "lib/render/fx_colors.h"
#include "lib/render/gl_coords.h"

#include "lib/render/cameras/gl_camera.h"

#include "lib/math/trig.h"
#include "lib/math/vector.h"

namespace anshub {

//****************************************************************************
// Infinite light source
//****************************************************************************

struct InfiniteArgs
{
  FColor  base_color_;
  Vector* normal_;

}; // struct InfiniteArgs

struct LightInfinite : public LightSource<InfiniteArgs>
{
  LightInfinite(cFColor& color, float intense, cVector& dir);
  ~LightInfinite() override { }

  void   Reset() override { direction_ = direction_copy_; }
  void   World2Camera(const GlCamera&, const TrigTable&) override;
  FColor Illuminate() const override;
  
  auto   GetDirection() const { return direction_; }  
  void   SetDirection(cVector&);

private:
  Vector  direction_;       // direction of light source
  Vector  direction_copy_;  // used to recover direction after frame 

}; // struct LightInfinite

}  // namespace anshub

#endif  // GL_LIGHT_INFINITE_H