// *************************************************************
// File:    gl_draw_triangles.cc
// Descr:   draws triangles arrays
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#include "gl_draw_triangles.h"

namespace anshub {

// Draws wired triangles

void draw::WiredTriangles(const V_Triangle& arr, Buffer& buf)
{
  int w = buf.Width();
  int h = buf.Height();
    
  for (const auto& t : arr)
  {
    if (!t.active_)
      continue;

    // As we haven`t filling, we get first vertexes color

    auto color = t.color_.GetARGB();

    // Now get pairs of vectors on the triangle face and draw lines

    auto p1 = t.vxs_[0].pos_;
    auto p2 = t.vxs_[1].pos_;

    if (segment2d::Clip(0, 0, w-1, h-1, p1.x, p1.y, p2.x, p2.y))
      draw::Line(p1.x, p1.y, p2.x, p2.y, color, buf);
      
    auto p3 = t.vxs_[1].pos_;
    auto p4 = t.vxs_[2].pos_;

    if (segment2d::Clip(0, 0, w-1, h-1, p3.x, p3.y, p4.x, p4.y))
      draw::Line(p3.x, p3.y, p4.x, p4.y, color, buf);

    auto p5 = t.vxs_[2].pos_;
    auto p6 = t.vxs_[0].pos_;

    if (segment2d::Clip(0, 0, w-1, h-1, p5.x, p5.y, p6.x, p6.y))
      draw::Line(p5.x, p5.y, p6.x, p6.y, color, buf);
  }
}

// Draws solid triangles

int draw::SolidTriangles(const V_Triangle& arr, Buffer& buf)
{
  int total {0};
  for (const auto& t : arr)
  {
    if (!t.active_)
      continue;

    auto p1 = t[0].pos_;
    auto p2 = t[1].pos_;
    auto p3 = t[2].pos_;
    auto t1 = t[0].texture_;
    auto t2 = t[1].texture_;
    auto t3 = t[2].texture_;

    if (t.texture_ != nullptr)
    {
      if (t.shading_ == Shading::CONST)
        draw::TexturedTriangle(t.texture_, p1, p2, p3, t1, t2, t3, buf);
      else if (t.shading_ == Shading::FLAT || t.shading_ == Shading::GOURANG)
        draw::TexturedTriangleFlatLight(
          t.texture_, p1, p2, p3, t1, t2, t3, t.color_.GetARGB(), buf);
    }
    else if (t.shading_ == Shading::GOURANG)
    {
      auto c1 = t[0].color_.GetARGB();
      auto c2 = t[1].color_.GetARGB();
      auto c3 = t[2].color_.GetARGB();
      draw::GourangTriangle(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, c1, c2, c3, buf);
    }
    else {
      auto color = t.color_.GetARGB();
      draw::SolidTriangle(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, color, buf);
    }
    ++total;
  }
  return total;
}

// Draws solid triangles with 1/z buffer

int draw::SolidTrianglesZ(
  const V_Triangle& arr, ZBuffer& zbuf, Buffer& buf)
{
  // Draw triangles

  int total {0};
  for (const auto& t : arr)
  {
    if (!t.active_)
      continue;

    auto p1 = t[0].pos_;
    auto p2 = t[1].pos_;
    auto p3 = t[2].pos_;
    
    if (t.texture_ != nullptr)
    {
      if (t.shading_ == Shading::CONST)
        draw::TexturedTriangle(t[0], t[1], t[2], t.texture_, zbuf, buf);
      else if (t.shading_ == Shading::FLAT || t.shading_ == Shading::GOURANG)
        draw::TexturedTriangleFlatLight(
          t[0], t[1], t[2], t.color_.GetARGB(), t.texture_, zbuf, buf);
    }
    else if (t.shading_ == Shading::GOURANG)
      draw::GourangTriangle(t[0], t[1], t[2], zbuf, buf);
    else
      draw::SolidTriangle(t[0], t[1], t[2], t.color_.GetARGB(), zbuf, buf);
    
    ++total;
  }
  return total;
}

} // namespace anshub