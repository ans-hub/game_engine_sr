// *************************************************************
// File:    gl_light.cc
// Descr:   represents light
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#include "gl_lights.h"

namespace anshub {

void light::Reset(Lights& lights)
{
  for (auto& l : lights.infinite_)
    l.Reset();
  for (auto& l : lights.point_)
    l.Reset();
}

// Convert Vectors inside light sources to camera coordinate

void light::World2Camera(Lights& lights, const GlCamera& cam)
{
  for (auto& l : lights.infinite_)
    l.World2Camera(cam);
  for (auto& l : lights.point_)
    l.World2Camera(cam);
}

// Lights object (in world coordinates)

void light::Object(GlObject& obj, Lights& lights)
{
  if (!obj.active_)
    return;
  
  auto& vxs = obj.GetCoords();
  std::vector<bool> used (vxs.size(), false);

  for (auto& face : obj.faces_)
  {
    if (!face.active_)
      continue;

    // If emission color then do nothing

    if (obj.shading_ == Shading::CONST)
      continue;

    // If triangle is flat shaded and not lighted already, then light it
    // and store color inside Triangle struct
    // Todo: here we may use something like gourang shading, but using not 
    // vxs normal, but normals to of face from each vertex to make soft
    // colors

    if (obj.shading_ == Shading::FLAT)
    {
      auto  bc = vxs[face[0]].color_;   // base color
      auto& cc = face.color_;           // color to change
      cc = {0.0f, 0.0f, 0.0f, 255.0f};

      auto normalized = math::Fzero(face.normal_.SquareLength() - 1.0f);
      if (!normalized)
        face.normal_.Normalize();

      for (auto& light : lights.ambient_)
        cc += light.Illuminate(bc);
      for (auto& light : lights.infinite_)
        cc += light.Illuminate(bc, face.normal_);
      for (auto& light : lights.point_)
        cc += light.Illuminate(bc, face.normal_, vxs[face[0]].pos_);
      cc.Clamp();
    }
    
    // If triangle is gourang shaded, then iterate over its vertixes colors,
    // check if its lighted, and store color in vertexes

    else if (obj.shading_ == Shading::GOURANG)
    {
      for (const auto& f : face.vxs_)
      {
        if (!used[f])
        {
          auto  bc = vxs[f].color_;
          auto& cc = vxs[f].color_;
          cc = {0.0f, 0.0f, 0.0f, 255.0f};
          used[f] = true;

          for (auto& light : lights.ambient_)
            cc += light.Illuminate(bc);
          for (auto& light : lights.infinite_)
            cc += light.Illuminate(bc, vxs[f].normal_);
          for (auto& light : lights.point_)
            cc += light.Illuminate(bc, vxs[f].normal_, vxs[f].pos_);
          cc.Clamp();
        }
      }
    }
  }
}

// Lights objects (in world coordinates)

void light::Objects(V_GlObject& arr, Lights& lights)
{
  for (auto& obj : arr)
    if (obj.active_) light::Object(obj, lights);
}

// Lights triangles (in camera coordinates)

void light::Triangles(V_Triangle& arr, Lights& lights)
{
  for (auto& tri : arr)
  {
    if (!tri.active_) continue;

    // If emission color then do nothing

    if (tri.shading_ == Shading::CONST)
      continue;

    // If triangle is flat shaded and not lighted already, then light it
    // and store color inside Triangle struct
    // Todo: here we may use something like gourang shading, but using not 
    // vxs normal, but normals to of face from each vertex to make soft
    // colors

    if (tri.shading_ == Shading::FLAT)
    {
      auto  bc = tri.vxs_[0].color_;   // base color
      auto& cc = tri.color_;           // color to change
      cc = {0.0f, 0.0f, 0.0f, 255.0f};

      auto normalized = math::Fzero(tri.normal_.SquareLength() - 1.0f);
      if (!normalized)
        tri.normal_.Normalize();

      for (auto& light : lights.ambient_)
        cc += light.Illuminate(bc);
      for (auto& light : lights.infinite_)
        cc += light.Illuminate(bc, tri.normal_);
      for (auto& light : lights.point_)
        cc += light.Illuminate(bc, tri.normal_, tri.vxs_[0].pos_);
      cc.Clamp();
    }
    
    // If triangle is gourang shaded, then iterate over its vertixes colors,
    // check if its lighted, and store color in vertexes

    else if (tri.shading_ == Shading::GOURANG)
    {
      for (auto& vx : tri.vxs_)
      {
        auto  bc = vx.color_;
        auto& cc = vx.color_;
        cc = {0.0f, 0.0f, 0.0f, 255.0f};

        for (auto& light : lights.ambient_)
          cc += light.Illuminate(bc);
        for (auto& light : lights.infinite_)
          cc += light.Illuminate(bc, vx.normal_);
        for (auto& light : lights.point_)
          cc += light.Illuminate(bc, vx.normal_, vx.pos_);
        cc.Clamp();
      }
    }
  }
}

} // namespace anshub