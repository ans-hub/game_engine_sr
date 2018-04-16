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

void light::World2Camera(Lights& lights, const GlCamera& cam, cTrigTable& trig)
{
  for (auto& l : lights.infinite_)
    l.World2Camera(cam, trig);
  for (auto& l : lights.point_)
    l.World2Camera(cam, trig);
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

    if (obj.shading_ == Shading::CONST)
      continue;

    // If triangle is flat shaded and not lighted already, then light it
    // and store color inside Triangle struct

    if (obj.shading_ == Shading::FLAT)
    {
      auto  bc = vxs[face[0]].color_;   // base color (note #1 after code)
      auto& cc = face.color_;           // color to change
      cc = {0.0f, 0.0f, 0.0f, bc.a_};

      if (!face.normal_.IsZero())
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
          cc = {0.0f, 0.0f, 0.0f, bc.a_};
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
 
    if (tri.shading_ == Shading::FLAT)
    {
      auto  bc = tri.vxs_[0].color_;      // base color (note #1 after code)
      auto& cc = tri.color_;              // color to change
      cc = {0.0f, 0.0f, 0.0f, bc.a_};

      if (!tri.normal_.IsZero())
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
        cc = {0.0f, 0.0f, 0.0f, bc.a_};

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

// Note #1 : since many 3d software support vertices color and ignore 
// face color I decide to use as color for face color of vertex[0].
// When we lights face color, it automatic replacements by the
// new color from vertex[0] in the next frame  