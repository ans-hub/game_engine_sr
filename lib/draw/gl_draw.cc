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

    auto v1 = vxs[f[0]].pos_;
    auto v2 = vxs[f[1]].pos_;
    auto v3 = vxs[f[2]].pos_;
    auto v1_back = v1;
    auto v2_back = v2;
    auto v3_back = v3;
    auto color = f.color_.GetARGB();

    if (segment2d::Clip(0, 0, w-1, h-1, v1.x, v1.y, v2.x, v2.y))
    {
      raster::Line(v1.x, v1.y, v2.x, v2.y, color, buf);
      v1 = v1_back;
      v2 = v2_back;
      v3 = v3_back;
    }
    if (segment2d::Clip(0, 0, w-1, h-1, v2.x, v2.y, v3.x, v3.y))
    {
      raster::Line(v2.x, v2.y, v3.x, v3.y, color, buf);
      v1 = v1_back;
      v2 = v2_back;
      v3 = v3_back;
    }    
    if (segment2d::Clip(0, 0, w-1, h-1, v3.x, v3.y, v1.x, v1.y))
      raster::Line(v3.x, v3.y, v1.x, v1.y, color, buf);

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

    // Make points aliases
    
    auto& p1 = vxs[f[0]].pos_;
    auto& p2 = vxs[f[1]].pos_;
    auto& p3 = vxs[f[2]].pos_;
    
    // Draw textured object

    if (obj.textured_)
    {
      auto& t1 = vxs[f[0]].texture_;
      auto& t2 = vxs[f[1]].texture_;
      auto& t3 = vxs[f[2]].texture_;
      auto* tex = obj.texture_.get();
      
      if (obj.shading_ == Shading::CONST)
      {
        raster_tri::TexturedAffine(p1, p2, p3, t1, t2, t3, tex, buf);      
      }
      else if (obj.shading_ == Shading::FLAT)
      {
        auto c = f.color_.GetARGB();        
        raster_tri::TexturedAffineFL(p1, p2, p3, t1, t2, t3, c, tex, buf);
      }
      else if (obj.shading_ == Shading::GOURANG)
      {
        auto c1 = vxs[f[0]].color_.GetARGB();
        auto c2 = vxs[f[1]].color_.GetARGB();
        auto c3 = vxs[f[2]].color_.GetARGB();
        raster_tri::TexturedAffineGR(p1, p2, p3, t1, t2, t3, c1, c2, c3, tex, buf);
      }
    }
    
    // Draw not textured object

    else
    {
      if (obj.shading_ == Shading::CONST || obj.shading_ == Shading::FLAT)
      {
        auto c = f.color_.GetARGB();
        raster_tri::SolidFL(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, c, buf);
      }
      else if (obj.shading_ == Shading::GOURANG)
      {
        auto c1 = vxs[f[0]].color_.GetARGB();
        auto c2 = vxs[f[1]].color_.GetARGB();
        auto c3 = vxs[f[2]].color_.GetARGB();
        raster_tri::SolidGR(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, c1, c2, c3, buf);
      }
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

int draw_triangles::Wired(const V_TrianglePtr& arr, Buffer& buf)
{
  int total {0};  
  int w {buf.Width()};
  int h {buf.Height()};

  for (const auto* t : arr)
  {
    if (!t->active_)
      continue;

    auto v1 = t->vxs_[0].pos_;
    auto v2 = t->vxs_[1].pos_;
    auto v3 = t->vxs_[2].pos_;
    auto v1_back = v1;
    auto v2_back = v2;
    auto v3_back = v3;
    auto color = t->color_.GetARGB();

    if (segment2d::Clip(0, 0, w-1, h-1, v1.x, v1.y, v2.x, v2.y))
    {
      raster::Line(v1.x, v1.y, v2.x, v2.y, color, buf);
      v1 = v1_back;
      v2 = v2_back;
      v3 = v3_back;
    }
    if (segment2d::Clip(0, 0, w-1, h-1, v2.x, v2.y, v3.x, v3.y))
    {
      raster::Line(v2.x, v2.y, v3.x, v3.y, color, buf);
      v1 = v1_back;
      v2 = v2_back;
      v3 = v3_back;
    }    
    if (segment2d::Clip(0, 0, w-1, h-1, v3.x, v3.y, v1.x, v1.y))
      raster::Line(v3.x, v3.y, v1.x, v1.y, color, buf);

    ++total;
  }
  return total;
}

// Draws solid triangles

int draw_triangles::Solid(const V_TrianglePtr& arr, Buffer& buf)
{
  int total_tris {0};

  for (const auto* t : arr)
  {
    if (!t->active_)
      continue;

    auto& p1 = t->vxs_[0].pos_;
    auto& p2 = t->vxs_[1].pos_;
    auto& p3 = t->vxs_[2].pos_;

    // Draw textured triangle

    if (t->texture_ != nullptr)
    {
      auto& t1 = t->vxs_[0].texture_;
      auto& t2 = t->vxs_[1].texture_;
      auto& t3 = t->vxs_[2].texture_;
      auto* tex = t->texture_;

      if (t->shading_ == Shading::CONST)
      {
        raster_tri::TexturedAffine(p1, p2, p3, t1, t2, t3, tex, buf);
      }
      else if (t->shading_ == Shading::FLAT)
      {
        auto c = t->color_.GetARGB();
        raster_tri::TexturedAffineFL(p1, p2, p3, t1, t2, t3, c, tex, buf);
      }
      
      else if (t->shading_ == Shading::GOURANG)
      {
        auto c1 = t->vxs_[0].color_.GetARGB();
        auto c2 = t->vxs_[1].color_.GetARGB();
        auto c3 = t->vxs_[2].color_.GetARGB();
        raster_tri::TexturedAffineGR(
          p1, p2, p3, t1, t2, t3, c1, c2, c3, tex, buf
        );
      }
    }

    // Draw colored triangle

    else
    {
      if (t->shading_ == Shading::CONST || t->shading_ == Shading::FLAT)
      {
        auto color = t->color_.GetARGB();
        raster_tri::SolidFL(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, color, buf);
      }
      else if (t->shading_ == Shading::GOURANG)
      {
        auto c1 = t->vxs_[0].color_.GetARGB();
        auto c2 = t->vxs_[1].color_.GetARGB();
        auto c3 = t->vxs_[2].color_.GetARGB();
        raster_tri::SolidGR(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, c1, c2, c3, buf);
      }
    }
    ++total_tris;
  }
  return total_tris;
}

// Draws solid triangles with 1/z buffer and returns total triangle which
// were been drawn

int draw_triangles::Solid(const V_TrianglePtr& arr, ZBuffer& zbuf, Buffer& buf)
{
  // Debug variables (we collect drawn pixels only from heavy weight functions)

  int total_px {0};     // total pixels drawn
  int total_tris {0};   // total triangles drawn

  for (const auto* t : arr)
  {
    if (!t->active_)
      continue;
    
    // Make aliases for vertices

    auto& v1 = t->vxs_[0];
    auto& v2 = t->vxs_[1];
    auto& v3 = t->vxs_[2];

    // Draw textured triangle

    if (t->texture_ != nullptr)
    {
      auto* tex = t->texture_;

      if (t->shading_ == Shading::CONST)
      {
        raster_tri::TexturedPerspective(v1, v2, v3, tex, zbuf, buf);
      }
      else if (t->shading_ == Shading::FLAT)
      {
        auto c = t->color_.GetARGB();        
        total_px += 
          raster_tri::TexturedPerspectiveFL(v1, v2, v3, c, tex, zbuf, buf);
      }
      else if (t->shading_ == Shading::GOURANG)
      {
        total_px +=
          raster_tri::TexturedPerspectiveGR(v1, v2, v3, tex, zbuf, buf);
      }
    }

    // Draw colored triangle
    
    else
    {
      if (t->shading_ == Shading::CONST || t->shading_ == Shading::FLAT)
      {
        auto c = t->color_.GetARGB();        
        raster_tri::SolidFL(v1, v2, v3, c, zbuf, buf);
      }
      else if (t->shading_ == Shading::GOURANG)
      {
        raster_tri::SolidGR(v1, v2, v3, zbuf, buf);
      }
    }
    ++total_tris;
  }
  return total_tris;
}

// Renders triangles and uses dist as chooser between affine and perspective
// correct texturing

int render::Solid(
  const V_TrianglePtr& arr, ZBuffer& zbuf, float dist, Buffer& buf)
{
  // Debug variables (we collect drawn pixels only from heavy weight functions)

  int total_px {0};     // total pixels drawn
  int total_tris {0};   // total triangles drawn

  for (const auto* t : arr)
  {
    if (!t->active_)
      continue;
    
    // Make aliases for vertices

    auto& v1 = t->vxs_[0];
    auto& v2 = t->vxs_[1];
    auto& v3 = t->vxs_[2];

    // Draw textured triangle

    if (t->texture_ != nullptr)
    {
      auto* tex = t->texture_;

      if (t->shading_ == Shading::CONST)
      {
        raster_tri::TexturedPerspective(v1, v2, v3, tex, zbuf, buf);
      }
      else if (t->shading_ == Shading::FLAT)
      {
        auto c = t->color_.GetARGB();        
        total_px += 
          raster_tri::TexturedPerspectiveFL(v1, v2, v3, c, tex, zbuf, buf);
      }
      else if (t->shading_ == Shading::GOURANG)
      {
        if (v1.pos_.z < dist)
          total_px +=
            raster_tri::TexturedPerspectiveGR(v1, v2, v3, tex, zbuf, buf);
        else
          total_px +=
            raster_tri::TexturedAffineGR(v1, v2, v3, tex, zbuf, buf);
      }
    }

    // Draw colored triangle
    
    else
    {
      if (t->shading_ == Shading::CONST || t->shading_ == Shading::FLAT)
      {
        auto c = t->color_.GetARGB();        
        raster_tri::SolidFL(v1, v2, v3, c, zbuf, buf);
      }
      else if (t->shading_ == Shading::GOURANG)
      {
        raster_tri::SolidGR(v1, v2, v3, zbuf, buf);
      }
    }
    ++total_tris;
  }
  return total_tris;
}


} // namespace anshub