// *************************************************************
// File:    point.h
// Descr:   represents point light source
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef GL_LIGHT_POINT_H
#define GL_LIGHT_POINT_H

#include "light_source.h"

#include "lib/draw/fx_colors.h"
#include "lib/draw/gl_aliases.h"
#include "lib/draw/gl_coords.h"

#include "lib/draw/cameras/gl_camera.h"

#include "lib/math/trig.h"
#include "lib/math/vector.h"

namespace anshub {

//****************************************************************************
// Point light source
//****************************************************************************

struct PointArgs    // holds arguments for Illuminate member func
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
  Vector  position_;          // direction of light source
  Vector  direction_;         // direction of light source
  Vector  position_copy_;     // used to recover direction after frame 
  Vector  direction_copy_;    // used to recover direction after frame 
  float   kc_;                // constant attenuation
  float   kl_;                // linear attenuation
  float   kq_;                // quadratic attenuation

}; // struct LightPoint

}  // namespace anshub

#endif  // GL_LIGHT_POINT_H