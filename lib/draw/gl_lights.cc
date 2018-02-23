// *************************************************************
// File:    gl_light.cc
// Descr:   represents light
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#include "gl_lights.h"

namespace anshub {

void light::Object(GlObject& obj, Lights& lights)
{
  if (!obj.active_)
    return;
  
  auto& vxs = obj.GetCoords();
  auto& colors = obj.GetColors();

  // Apply lighting to each triangle

  for (auto& tri : obj.triangles_)
  {
    if (tri.attrs_ & Triangle::HIDDEN)
      continue;

    // Copy color information into triangle struct

    tri.c1_ = FColor{0.0f, 0.0f, 0.0f};
    tri.c2_ = FColor{0.0f, 0.0f, 0.0f};
    tri.c3_ = FColor{0.0f, 0.0f, 0.0f};
    auto& bc1 = colors[tri.f1_];
    auto& bc2 = colors[tri.f1_];
    auto& bc3 = colors[tri.f1_];

    // Calc ambient lighting (preffered only 1 source of ambient light)

    for (const auto& l : lights.ambient_)
    {
      auto amb1 = (bc1 * l.color_ * l.intense_) / 256.0f;
      auto amb2 = (bc2 * l.color_ * l.intense_) / 256.0f;
      auto amb3 = (bc3 * l.color_ * l.intense_) / 256.0f;
      tri.c1_ += amb1;
      tri.c2_ += amb2;
      tri.c3_ += amb3;
    }

    // Calc infinite lighting

    // Calc normal to the face

    auto p0 = vxs[tri.f1_];
    auto p1 = vxs[tri.f2_];
    auto p2 = vxs[tri.f3_];
    Vector u {p0, p1};
    Vector v {p0, p2};
    Vector n = vector::CrossProduct(u, v);
    n.Normalize();

    for (const auto& l : lights.infinite_)
    {
      Vector dir = l.direction_ * (-1);    
      auto prod = vector::DotProduct(dir, n);
      if (prod < 0) prod = 0;
      auto inf1 = (bc1 * l.color_ * l.intense_ * prod) / 256.0f;
      auto inf2 = (bc2 * l.color_ * l.intense_ * prod) / 256.0f;
      auto inf3 = (bc3 * l.color_ * l.intense_ * prod) / 256.0f;
      tri.c1_ += inf1;
      tri.c2_ += inf2;
      tri.c3_ += inf3;
    }

    // Normalize colors

    tri.c1_.Clamp();
    tri.c2_.Clamp();
    tri.c3_.Clamp();
  }
}

void light::Objects(GlObjects& arr, Lights& lights)
{
  for (auto& obj : arr)
  {
    light::Object(obj, lights); 
  }
}

} // namespace anshub