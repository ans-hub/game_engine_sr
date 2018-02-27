// *************************************************************
// File:    gl_light.cc
// Descr:   represents light
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#include "gl_lights.h"

namespace anshub {

// Lights object

void light::Object(GlObject& obj, Lights& lights)
{
  if (!obj.active_)
    return;
  
  auto& colors = obj.GetColors();
  auto& vxs = obj.GetCoords();
  std::vector<bool> used (obj.colors_trans_.size(), false);

  for (auto& tri : obj.triangles_)
  {
    if (tri.attrs_ & Triangle::HIDDEN)
      continue;

    // If emission color then do nothing

    if ((tri.attrs_ & Triangle::CONST_SHADING))
      continue;

    // If triangle is flat shaded and not lighted already, then light it
    // and store color inside Triangle struct
    // Todo: here we may use something like gourang shading, but using not 
    // vxs normal, but normals to of face from each vertex to make soft
    // colors

    if (tri.attrs_ & Triangle::FLAT_SHADING)
    {
      auto  bc = colors[tri.f1_];
      auto& cc = tri.face_color_;
      cc = {0.0f, 0.0f, 0.0f};

      auto normalized = math::Fzero(tri.face_normal_.SquareLength() - 1.0f);
      if (!normalized)
        tri.face_normal_.Normalize();

      for (auto& light : lights.ambient_)
        cc += light.Illuminate(bc);
      for (auto& light : lights.infinite_)
        cc += light.Illuminate(bc, tri.face_normal_);
      for (auto& light : lights.point_)
        cc += light.Illuminate(bc, tri.face_normal_, vxs[tri.f1_]);
      cc.Clamp();
    }
    
    // If triangle is gourang shaded, then iterate over its vertixes colors,
    // check if its lighted, and store color in vertexes

    else if (tri.attrs_ & Triangle::GOURANG_SHADING)
    {
      std::vector<int> faces {tri.f1_, tri.f2_, tri.f3_};
      for (const auto& face : faces)
      if (!used[face])
      {
        auto  bc = colors[face];
        auto& cc = colors[face];
        cc = {0.0f, 0.0f, 0.0f};
        used[face] = true;

        for (auto& light : lights.ambient_)
          cc += light.Illuminate(bc);
        for (auto& light : lights.infinite_)
          cc += light.Illuminate(bc, obj.vxs_normals_[face]);
        for (auto& light : lights.point_)
          cc += light.Illuminate(bc, obj.vxs_normals_[face], vxs[face]);
        cc.Clamp();
      }
    }
  }
}

// Lights objects

void light::Objects(GlObjects& arr, Lights& lights)
{
  for (auto& obj : arr)
    light::Object(obj, lights);
}

} // namespace anshub