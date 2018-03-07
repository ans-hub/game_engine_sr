// *************************************************************
// File:    gl_draw_objects.cc
// Descr:   draws objects
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#include "gl_draw_objects.h"

namespace anshub {

// Draws wired object

void draw::WiredObject(const GlObject& obj, Buffer& buf)
{
  if (!obj.active_)
    return;

  auto& vxs = obj.GetCoords();
  int w = buf.Width();
  int h = buf.Height();
    
  for (const auto& t : obj.faces_)
  {
    if (!t.active_)
      continue;

    // As we haven`t filling, we get first vertexes color

    auto color = t.color_.GetARGB();

    // Now get pairs of vectors on the triangle face and draw lines

    auto p1 = vxs[t[0]];
    auto p2 = vxs[t[1]];

    if (segment2d::Clip(0, 0, w-1, h-1, p1.pos_.x, p1.pos_.y, p2.pos_.x, p2.pos_.y))
      draw::Line(p1.pos_.x, p1.pos_.y, p2.pos_.x, p2.pos_.y, color, buf);
      
    auto p3 = vxs[t[1]];
    auto p4 = vxs[t[2]];

    if (segment2d::Clip(0, 0, w-1, h-1, p3.pos_.x, p3.pos_.y, p4.pos_.x, p4.pos_.y))
      draw::Line(p3.pos_.x, p3.pos_.y, p4.pos_.x, p4.pos_.y, color, buf);

    auto p5 = vxs[t[2]];
    auto p6 = vxs[t[0]];

    if (segment2d::Clip(0, 0, w-1, h-1, p5.pos_.x, p5.pos_.y, p6.pos_.x, p6.pos_.y))
      draw::Line(p5.pos_.x, p5.pos_.y, p6.pos_.x, p6.pos_.y, color, buf);
  }
}

// Draws solid object

int draw::SolidObject(const GlObject& obj, Buffer& buf)
{
  int total {0};
  auto& vxs = obj.GetCoords();

  if (!obj.active_)
    return total;
  
  for (const auto& face : obj.faces_)
  {
    if (!face.active_)
      continue;

    auto p1 = vxs[face[0]].pos_;
    auto p2 = vxs[face[1]].pos_;
    auto p3 = vxs[face[2]].pos_;
    
    if (obj.textured_)
    {
      auto t1 = vxs[face[0]].texture_;
      auto t2 = vxs[face[1]].texture_;
      auto t3 = vxs[face[2]].texture_;
      draw::TexturedTriangleFlatLight(
        obj.texture_.get(), p1, p2, p3, t1, t2, t3, face.color_.GetARGB(), buf);
    }
    else if (obj.shading_ == Shading::GOURANG)
    {
      auto c1 = vxs[face[0]].color_.GetARGB();
      auto c2 = vxs[face[1]].color_.GetARGB();
      auto c3 = vxs[face[2]].color_.GetARGB();
      draw::GourangTriangle(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, c1, c2, c3, buf);
    }
    else {
      auto color = face.color_.GetARGB();
      draw::SolidTriangle(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, color, buf);
    }
    ++total;
  }
  return total;
}

// Draws object normals in debug purposes
// Here we build segments of normals like: obj.vx[i] is start, ends.vx[i] is end

void draw::ObjectNormals(
  const GlObject& obj, const V_Vertex& ends, uint color, Buffer& buf)
{
  auto& vxs = obj.GetCoords();
  auto w = buf.Width();
  auto h = buf.Height();
  for (std::size_t i = 0; i < vxs.size(); ++i)
  {
    auto st_x = vxs[i].pos_.x;
    auto st_y = vxs[i].pos_.y;
    auto en_x = ends[i].pos_.x;
    auto en_y = ends[i].pos_.y;
    if (segment2d::Clip(0, 0, w-1, h-1, st_x, st_y, en_x, en_y))
        draw::Line(st_x, st_y, en_x, en_y, color, buf);
  }
}

} // namespace anshub