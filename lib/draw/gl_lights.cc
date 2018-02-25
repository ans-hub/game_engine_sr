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
  
  auto& colors = obj.GetColors();

  // Iterate over each triangle, check if it visible. And if not, then
  // drop colors to 0

  for (auto& tri : obj.triangles_)
  {
    if (tri.attrs_ & Triangle::HIDDEN)
      continue;

    if (tri.attrs_ & Triangle::CONST_SHADING)
    {
      light::Emission(tri, colors);
      continue;
    }

    // Reset total lighting of current triangle
    
    tri.c1_ = FColor{0.0f, 0.0f, 0.0f};
    tri.c2_ = FColor{0.0f, 0.0f, 0.0f};
    tri.c3_ = FColor{0.0f, 0.0f, 0.0f};

    // Remember source color of triangle

    auto bc1 = colors[tri.f1_];
    auto bc2 = colors[tri.f2_];
    auto bc3 = colors[tri.f3_];

    if (tri.attrs_ & Triangle::FLAT_SHADING)
    {
      tri.face_normal_.Normalize();
      light::Ambient(tri, bc1, bc2, bc3, lights);
      light::flat::Infinite(tri, bc1, bc2, bc3, lights);
      light::flat::Point(tri, bc1, bc2, bc3, lights);
    }
    else if (tri.attrs_ & Triangle::GOURANG_SHADING)
    {
      light::Ambient(tri, bc1, bc2, bc3, lights);
      light::gourang::Infinite(tri, obj.vxs_normals_, bc1, bc2, bc3, lights);
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
    light::Object(obj, lights);
}

void light::Emission(Triangle& tri, FColors& colors)
{
  tri.c1_ = colors[tri.f1_];
  tri.c2_ = colors[tri.f2_];
  tri.c3_ = colors[tri.f3_];
}

void light::Ambient(
  Triangle& tri, cFColor& bc1, cFColor& bc2, cFColor& bc3, Lights& lights)
{
  for (const auto& l : lights.ambient_)
  {
    auto amb1 = (bc1 * l.color_ * l.intense_) / 256.0f;
    auto amb2 = (bc2 * l.color_ * l.intense_) / 256.0f;
    auto amb3 = (bc3 * l.color_ * l.intense_) / 256.0f;
    tri.c1_ += amb1;
    tri.c2_ += amb2;
    tri.c3_ += amb3;
  }
}

void light::flat::Infinite(
  Triangle& tri, cFColor& bc1, cFColor& bc2, cFColor& bc3, Lights& lights)
{
  for (const auto& l : lights.infinite_)
  {
    auto dir = l.direction_ * (-1);
    auto prod = vector::DotProduct(dir, tri.face_normal_); 
    if (prod < 0) prod = 0;

    auto inf1 = (bc1 * l.color_ * l.intense_ * prod) / 256.0f;
    auto inf2 = (bc2 * l.color_ * l.intense_ * prod) / 256.0f;
    auto inf3 = (bc3 * l.color_ * l.intense_ * prod) / 256.0f;
    tri.c1_ += inf1;
    tri.c2_ += inf2;
    tri.c3_ += inf3;
  }
}

void light::flat::Point(
  Triangle& tri, cFColor& bc1, cFColor& bc2, cFColor& bc3, Lights& lights)
{
  for (const auto& l : lights.point_)
  {
    // auto dir = l.direction_ * (-1);
    // auto prod = vector::DotProduct(dir, tri.face_normal_); 
    // if (prod < 0) prod = 0;

    // auto inf1 = (bc1 * l.color_ * l.intense_ * prod) / 256.0f;
    // auto inf2 = (bc2 * l.color_ * l.intense_ * prod) / 256.0f;
    // auto inf3 = (bc3 * l.color_ * l.intense_ * prod) / 256.0f;
    // tri.c1_ += inf1;
    // tri.c2_ += inf2;
    // tri.c3_ += inf3;
  }
}

void light::gourang::Infinite(
  Triangle& tri, Vectors& norms, cFColor& bc1, cFColor& bc2, cFColor& bc3,
  Lights& lights)
{
  for (const auto& l : lights.infinite_)
  {
    auto dir = l.direction_ * (-1);

    auto prod1 = vector::DotProduct(dir, norms[tri.f1_]); 
    auto prod2 = vector::DotProduct(dir, norms[tri.f2_]); 
    auto prod3 = vector::DotProduct(dir, norms[tri.f3_]); 

    if (prod1 < 0) prod1 = 0;
    if (prod2 < 0) prod2 = 0;
    if (prod3 < 0) prod3 = 0;
    
    auto inf1 = (bc1 * l.color_ * l.intense_ * prod1) / 256.0f;
    auto inf2 = (bc2 * l.color_ * l.intense_ * prod2) / 256.0f;
    auto inf3 = (bc3 * l.color_ * l.intense_ * prod3) / 256.0f;
    tri.c1_ += inf1;
    tri.c2_ += inf2;
    tri.c3_ += inf3;
  }
}

} // namespace anshub