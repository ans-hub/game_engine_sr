// *************************************************************
// File:    gl_draw.cc
// Descr:   draws objects and triangles arrays
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#include "gl_draw.h"

namespace anshub {

// Draws wired object

int draw_object::Wired(const GlObject& obj, Buffer& buf)
{
  int total {0};

  if (!obj.active_)
    return total;

  int w {buf.Width()};
  int h {buf.Height()};
  auto& vxs = obj.GetCoords();
    
  for (const auto& f : obj.faces_)
  {
    if (!f.active_)
      continue;

    int x1 = vxs[f[0]].pos_.x;
    int x2 = vxs[f[1]].pos_.x;
    int x3 = vxs[f[2]].pos_.x;
    int y1 = vxs[f[0]].pos_.y;
    int y2 = vxs[f[1]].pos_.y;
    int y3 = vxs[f[2]].pos_.y;
    auto color = f.color_.GetARGB();

    if (segment2d::Clip(0, 0, w-1, h-1, x1, y1, x2, y2))
      raster::Line(x1, y1, x2, y2, color, buf);
    if (segment2d::Clip(0, 0, w-1, h-1, x2, y2, x3, y3))
      raster::Line(x2, y2, x3, y3, color, buf);
    if (segment2d::Clip(0, 0, w-1, h-1, x3, y3, x1, y1))
      raster::Line(x3, y3, x1, y1, color, buf);
    
    ++total;
  }
  return total;
}

// Draws solid object

int draw_object::Solid(const GlObject& obj, Buffer& buf)
{
  int total {0};

  if (!obj.active_)
    return total;

  auto& vxs = obj.GetCoords();
  
  for (const auto& f : obj.faces_)
  {
    if (!f.active_)
      continue;

    auto p1 = vxs[f[0]].pos_;
    auto p2 = vxs[f[1]].pos_;
    auto p3 = vxs[f[2]].pos_;
    
    // Draw textured object

    if (obj.textured_)
    {
      auto t1 = vxs[f[0]].texture_;
      auto t2 = vxs[f[1]].texture_;
      auto t3 = vxs[f[2]].texture_;
      auto* tex = obj.texture_.get();
      auto c = f.color_.GetARGB();
      
      if (obj.shading_ == Shading::CONST)
        raster::TexturedTriangle(p1, p2, p3, t1, t2, t3, tex, buf);

      else if (obj.shading_ == Shading::FLAT || obj.shading_ == Shading::GOURANG)
        raster::TexturedTriangleFL(p1, p2, p3, t1, t2, t3, c, tex, buf);
    }
    
    // Draw colored object

    else if (obj.shading_ == Shading::GOURANG)
    {
      auto c1 = vxs[f[0]].color_.GetARGB();
      auto c2 = vxs[f[1]].color_.GetARGB();
      auto c3 = vxs[f[2]].color_.GetARGB();
      raster::GourangTriangle(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, c1, c2, c3, buf);
    }
    else {
      auto color = f.color_.GetARGB();
      raster::SolidTriangle(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, color, buf);
    }
    ++total;
  }
  return total;
}

// Draws object normals in debug purposes
// Here we build segments of normals like: obj.vx[i] is start, ends.vx[i] is end

void draw_object::Normals(
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
        raster::Line(st_x, st_y, en_x, en_y, color, buf);
  }
}

// Draws wired triangles

int draw_triangles::Wired(const V_Triangle& arr, Buffer& buf)
{
  int total {0};  
  int w {buf.Width()};
  int h {buf.Height()};

  for (const auto& t : arr)
  {
    if (!t.active_)
      continue;

    int x1 = t[0].pos_.x;
    int x2 = t[1].pos_.x;
    int x3 = t[2].pos_.x;
    int y1 = t[0].pos_.y;
    int y2 = t[1].pos_.y;
    int y3 = t[2].pos_.y;
    auto color = t.color_.GetARGB();

    if (segment2d::Clip(0, 0, w-1, h-1, x1, y1, x2, y2))
      raster::Line(x1, y1, x2, y2, color, buf);
    if (segment2d::Clip(0, 0, w-1, h-1, x2, y2, x3, y3))
      raster::Line(x2, y2, x3, y3, color, buf);
    if (segment2d::Clip(0, 0, w-1, h-1, x3, y3, x1, y1))
      raster::Line(x3, y3, x1, y1, color, buf);
    
    ++total;
  }
  return total;
}

// Draws solid triangles

int draw_triangles::Solid(const V_Triangle& arr, Buffer& buf)
{
  int total {0};

  for (const auto& t : arr)
  {
    if (!t.active_)
      continue;

    auto p1 = t[0].pos_;
    auto p2 = t[1].pos_;
    auto p3 = t[2].pos_;

    // Draw textured triangle

    if (t.texture_ != nullptr)
    {
      auto t1 = t[0].texture_;
      auto t2 = t[1].texture_;
      auto t3 = t[2].texture_;
      auto c = t.color_.GetARGB();

      if (t.shading_ == Shading::CONST)
        raster::TexturedTriangle(p1, p2, p3, t1, t2, t3, t.texture_, buf);

      else if (t.shading_ == Shading::FLAT || t.shading_ == Shading::GOURANG)
        raster::TexturedTriangleFL( p1, p2, p3, t1, t2, t3, c, t.texture_, buf);
    }

    // Draw colored triangle

    else if (t.shading_ == Shading::GOURANG)
    {
      auto c1 = t[0].color_.GetARGB();
      auto c2 = t[1].color_.GetARGB();
      auto c3 = t[2].color_.GetARGB();
      raster::GourangTriangle(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, c1, c2, c3, buf);
    }
    else {
      auto color = t.color_.GetARGB();
      raster::SolidTriangle(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, color, buf);
    }
    ++total;
  }
  return total;
}

// Draws solid triangles with 1/z buffer

int draw_triangles::Solid(const V_Triangle& arr, ZBuffer& zbuf, Buffer& buf)
{
  int total {0};

  for (const auto& t : arr)
  {
    if (!t.active_)
      continue;

    // Draw textured triangle
    
    if (t.texture_ != nullptr)
    {
      auto c = t.color_.GetARGB();

      if (t.shading_ == Shading::CONST)
        raster::TexturedTriangle(t[0], t[1], t[2], t.texture_, zbuf, buf);

      else if (t.shading_ == Shading::FLAT || t.shading_ == Shading::GOURANG)
        raster::TexturedTriangleFL(t[0], t[1], t[2], c, t.texture_, zbuf, buf);
    }

    // Draw colored triangle
    
    else if (t.shading_ == Shading::GOURANG)
      raster::GourangTriangle(t[0], t[1], t[2], zbuf, buf);
    else
      raster::SolidTriangle(t[0], t[1], t[2], t.color_.GetARGB(), zbuf, buf);
    
    ++total;
  }
  return total;
}

} // namespace anshub