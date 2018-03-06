// *************************************************************
// File:    gl_triangle.cc
// Descr:   drawable triangle struct for renderer
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#include "gl_triangle.h"

namespace anshub {

// Makes container of triangles references array. This is simple array of
// references.

V_Triangle triangles::MakeContainer()
{
  return V_Triangle {};
}

// Add references to triangles from objects to triangles container

void triangles::AddFromObject(GlObject& obj, V_Triangle& triangles)
{
  if (obj.active_)
  {
    auto& vxs = obj.GetCoords();
    if (obj.textured_) {
      for (auto& face : obj.faces_)
      {
        if (face.active_)
          triangles.emplace_back(vxs, obj.shading_, face, obj.texture_.get());
      }
    } 
    else {
      for (auto& face : obj.faces_)
      {
        if (face.active_)
          triangles.emplace_back(vxs, obj.shading_, face, nullptr);
      }
    }
  }
}

// Add references to triangles from objects to triangles container

void triangles::AddFromObjects(V_GlObject& arr, V_Triangle& triangles)
{
  for (auto& obj : arr)
  {
    if (obj.active_)
    {
      auto& vxs = obj.GetCoords();
      if (obj.textured_) {
        for (auto& face : obj.faces_)
        {
          if (face.active_)
            triangles.emplace_back(vxs, obj.shading_, face, obj.texture_.get());
        }
      } 
      else {
        for (auto& face : obj.faces_)
        {
          if (face.active_)
            triangles.emplace_back(vxs, obj.shading_, face, nullptr);
        }
      }
    }
  }
}

// Hides invisible faces to viewpoint. Works as the same function in
// ::object namespace

int triangles::RemoveHiddenSurfaces(V_Triangle& arr, const GlCamera& cam)
{
  int cnt {0};
  for (auto& tri : arr)
  {
    Vector u {tri[0].pos_, tri[1].pos_};
    Vector v {tri[0].pos_, tri[2].pos_};
    Vector n = vector::CrossProduct(u,v);   // normal to u and v
    Vector c {tri[0].pos_, cam.vrp_};       // view vector

    auto prod = vector::DotProduct(c,n);
    if (math::FlessZero(prod))
    {
      tri.active_ = false;
      ++cnt;
    }
  }
  return cnt;  
}

// Reset attributes of all triangles
// todo : test it

void triangles::ResetAttributes(V_Triangle& arr)
{
  for (auto& tri : arr) {
    if (!tri.active_)
      tri.active_ = true;
  }
}

// Apply matrix to all triangles in array
// todo : test it

void triangles::ApplyMatrix(const Matrix<4,4>& mx, V_Triangle& arr)
{
  for (auto& tri : arr)
  {
    auto v1 = matrix::Multiplie(tri[0].pos_, mx);
    auto v2 = matrix::Multiplie(tri[1].pos_, mx);
    auto v3 = matrix::Multiplie(tri[2].pos_, mx);
    tri[0].pos_ = v1;
    tri[1].pos_ = v2;
    tri[2].pos_ = v3;
  }
}

void triangles::World2Camera(V_Triangle& arr, const GlCamera& cam)
{
  for (auto& tri : arr)
  {
    V_Vertex vxs {tri[0], tri[1], tri[2]};
    coords::World2Camera(vxs, cam.vrp_, cam.dir_, cam.trig_);
    tri[0].pos_ = vxs[0].pos_;
    tri[1].pos_ = vxs[1].pos_;
    tri[2].pos_ = vxs[2].pos_;
  }
}

void triangles::Camera2Persp(V_Triangle& arr, const GlCamera& cam)
{
  for (auto& tri : arr)
  {
    V_Vertex vxs {tri[0], tri[1], tri[2]};
    coords::Camera2Persp(vxs, cam.dov_, cam.ar_);
    tri[0].pos_ = vxs[0].pos_;
    tri[1].pos_ = vxs[1].pos_;
    tri[2].pos_ = vxs[2].pos_;
  }
}

void triangles::Homogenous2Normal(V_Triangle& arr)
{
  for (auto& tri : arr)
  {
    vector::ConvertFromHomogeneous(tri[0].pos_);
    vector::ConvertFromHomogeneous(tri[1].pos_);
    vector::ConvertFromHomogeneous(tri[2].pos_);
  }
}

void triangles::Persp2Screen(V_Triangle& arr, const GlCamera& cam)
{
  for (auto& tri : arr)
  {
    V_Vertex vxs {tri[0], tri[1], tri[2]};
    coords::Persp2Screen(vxs, cam.wov_, cam.scr_w_, cam.scr_h_);
    tri[0].pos_ = vxs[0].pos_;
    tri[1].pos_ = vxs[1].pos_;
    tri[2].pos_ = vxs[2].pos_;
  }
}

// Z-sorting of triangles using average z coordinate. We should do this
// before acsonometric projection

void triangles::SortZAvg(V_Triangle& arr)
{
  std::sort(arr.begin(), arr.end(), [](auto& t1, auto& t2)
  {
    auto avg_z1 {
      0.3333333f * (t1.vxs_[0].pos_.z + t1.vxs_[1].pos_.z + t1.vxs_[2].pos_.z)};
    auto avg_z2 {
      0.3333333f * (t2.vxs_[0].pos_.z + t2.vxs_[1].pos_.z + t2.vxs_[2].pos_.z)};
    return avg_z1 > avg_z2;
  });
}

// The same as above but sorts using far z coordinate

void triangles::SortZFar(V_Triangle& arr)
{
  std::sort(arr.begin(), arr.end(), [](auto& t1, auto& t2)
  {
    auto avg_z1 {std::max(t1.vxs_[0].pos_.z, t1.vxs_[1].pos_.z)};
    avg_z1 = std::max(avg_z1, t1.vxs_[2].pos_.z);
    auto avg_z2 {std::max(t2.vxs_[0].pos_.z, t2.vxs_[1].pos_.z)};
    avg_z2 = std::max(avg_z2, t2.vxs_[2].pos_.z);
    return avg_z1 > avg_z2;
  });
}

} // namespace anshub