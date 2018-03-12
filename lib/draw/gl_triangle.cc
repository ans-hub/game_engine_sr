// *************************************************************
// File:    gl_triangle.cc
// Descr:   drawable triangle struct for renderer
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#include "gl_triangle.h"

namespace anshub {

// Makes container of Triangles with supposed capacity. If we would use
// this container right (i.e. will resize it to 0, but not clear every 
// loop tick), then start capacity may be 0

V_Triangle triangles::MakeBaseContainer(int capacity)
{
  V_Triangle res {};
  res.reserve(capacity);
  return res;
}

// Makes container of Triangles with supposed capacity. If we would use
// this container right (i.e. will resize it to 0, but not clear every 
// loop tick), then start capacity may be 0

V_TrianglePtr triangles::MakePtrsContainer(int capacity)
{
  V_TrianglePtr res {};
  res.reserve(capacity);
  return res;
}

// Makes container of Triangles pointers for quick sort and other perfomance
// purposes

void triangles::MakePointers(V_Triangle& base, V_TrianglePtr& ptr)
{
  for (auto& tri : base)
    ptr.emplace_back(&tri);
}

// Add references to triangles from objects to triangles container

void triangles::AddFromObject(GlObject& obj, V_Triangle& triangles)
{
  if (!obj.active_)
    return;

  auto& vxs = obj.GetCoords();
  Bitmap* tex = obj.textured_ ? obj.texture_.get() : nullptr;
 
  for (auto& face : obj.faces_)
    if (face.active_)
      triangles.emplace_back(vxs, obj.shading_, face, tex);
}

// Add references to triangles from objects to triangles container

void triangles::AddFromObjects(V_GlObject& arr, V_Triangle& triangles)
{
  for (auto& obj : arr)
  {
    if (!obj.active_)
      continue;

    auto& vxs = obj.GetCoords();
    Bitmap* tex = obj.textured_ ? obj.texture_.get() : nullptr;
  
    for (auto& face : obj.faces_)
      if (face.active_)
        triangles.emplace_back(vxs, obj.shading_, face, tex);
  }
}

// Hides invisible faces to viewpoint. Works as the same function in
// ::object namespace

int triangles::RemoveHiddenSurfaces(V_Triangle& arr, const GlCamera& cam)
{
  int cnt {0};
  for (auto& tri : arr)
  {
    // Compute trianglenormal
    
    if (tri.normal_.IsZero())
    {
      Vector u {tri[0].pos_, tri[1].pos_};
      Vector v {tri[0].pos_, tri[2].pos_};
      tri.normal_ = vector::CrossProduct(u,v);
      tri.normal_.Normalize();
    }

    // Compute vector of view (this is just potential view, not fact)
    
    Vector view {tri[0].pos_, cam.vrp_};
    view.Normalize();

    // If angle between view vector and normal < 90 deg, then face is invisible    

    auto prod = vector::DotProduct(view, tri.normal_);
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
    tri.normal_.Zero();
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

// Sorts triangles by average z coordinate (first - nearest). We should do this
// before acsonometric projection.

void triangles::SortZAvg(V_TrianglePtr& arr)
{
  std::sort(arr.begin(), arr.end(), [](auto* t1, auto* t2)
  {
    auto avg_z1 {
      0.3333333f * (t1->vxs_[0].pos_.z + t1->vxs_[1].pos_.z + t1->vxs_[2].pos_.z)};
    auto avg_z2 {
      0.3333333f * (t2->vxs_[0].pos_.z + t2->vxs_[1].pos_.z + t2->vxs_[2].pos_.z)};
    return avg_z1 > avg_z2;
  });
}

// The same as above but sorts using far z coordinate

void triangles::SortZFar(V_TrianglePtr& arr)
{
  std::sort(arr.begin(), arr.end(), [](auto* t1, auto* t2)
  {
    auto avg_z1 {std::max(t1->vxs_[0].pos_.z, t1->vxs_[1].pos_.z)};
    avg_z1 = std::max(avg_z1, t1->vxs_[2].pos_.z);
    auto avg_z2 {std::max(t2->vxs_[0].pos_.z, t2->vxs_[1].pos_.z)};
    avg_z2 = std::max(avg_z2, t2->vxs_[2].pos_.z);
    return avg_z1 > avg_z2;
  });
}

// Invert sorts triangles using average z coordinate (first - farthest). We should
// do this before acsonometric projection

void triangles::SortZAvgInv(V_TrianglePtr& arr)
{
  std::sort(arr.begin(), arr.end(), [](auto* t1, auto* t2)
  {
    auto avg_z1 {
      0.3333333f * (t1->vxs_[0].pos_.z + t1->vxs_[1].pos_.z + t1->vxs_[2].pos_.z)};
    auto avg_z2 {
      0.3333333f * (t2->vxs_[0].pos_.z + t2->vxs_[1].pos_.z + t2->vxs_[2].pos_.z)};
    return avg_z1 < avg_z2;
  });
}

// The same as above but sorts using far z coordinate

void triangles::SortZFarInv(V_TrianglePtr& arr)
{
  std::sort(arr.begin(), arr.end(), [](auto& t1, auto& t2)
  {
    auto avg_z1 {std::max(t1->vxs_[0].pos_.z, t1->vxs_[1].pos_.z)};
    avg_z1 = std::max(avg_z1, t1->vxs_[2].pos_.z);
    auto avg_z2 {std::max(t2->vxs_[0].pos_.z, t2->vxs_[1].pos_.z)};
    avg_z2 = std::max(avg_z2, t2->vxs_[2].pos_.z);
    return avg_z1 < avg_z2;
  });
}


} // namespace anshub