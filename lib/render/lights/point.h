// *************************************************************
// File:    point.h
// Descr:   represents point light source
// Author:  Novoselov Anton @ 2017
// *************************************************************

#ifndef GL_LIGHT_POINT_H
#define GL_LIGHT_POINT_H

#include "light_source.h"

#include "lib/render/fx_colors.h"
#include "lib/render/gl_aliases.h"
#include "lib/render/gl_coords.h"

#include "lib/render/cameras/gl_camera.h"

#include "lib/math/trig.h"
#include "lib/math/vector.h"

namespace anshub {

//****************************************************************************
// Point light source
//****************************************************************************

struct PointArgs
{
  FColor  base_color_;
  Vector* normal_;
  Vector* destination_;

}; // struct PointArgs

struct LightPoint : public LightSource<PointArgs>
{
  LightPoint(cFColor&, float intense, cVector& pos, cVector& dir);
  LightPoint(cFColor&, float, cVector&, cVector&, float kc, float kl, float kq);
  ~LightPoint() override { }
  
  void   Reset() override;
  void   World2Camera(const GlCamera&, const TrigTable&) override;
  FColor Illuminate() const override;
  
  auto   GetPosition() const { return position_; }
  auto   GetDirection() const { return direction_; }
  void   SetPosition(cVector&);
  void   SetDirection(cVector&);

private:
  Vector  position_;          // position of light source
  Vector  direction_;         // direction of light source 
  Vector  position_copy_;     // used to recover position after frame 
  Vector  direction_copy_;    // used to recover direction after frame 
  float   kc_;                // constant attenuation
  float   kl_;                // linear attenuation
  float   kq_;                // quadratic attenuation

}; // struct LightPoint

}  // namespace anshub

#endif  // GL_LIGHT_POINT_H