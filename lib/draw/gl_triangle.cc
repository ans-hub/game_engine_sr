// *************************************************************
// File:    gl_triangle.cc
// Descr:   drawable triangle struct for renderer
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#include "gl_triangle.h"

namespace anshub {

Triangle::Triangle()
  : active_{false}
  , shading_{}
  , vxs_{}
  , normal_{}
  , color_{}
  , textures_{nullptr}
{ }

Triangle::Triangle(
  const V_Vertex& vxs, Shading shading, const Face& f, V_Bitmap& tex
)
  : active_{true}
  , shading_{shading}
  , vxs_{ {
      vxs[f.vxs_[0]], vxs[f.vxs_[1]], vxs[f.vxs_[2]]
    } }
  , normal_{f.normal_}
  , color_{f.color_}
  , textures_{&tex}
{ }

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
    if (tri.active_)
      ptr.emplace_back(&tri);
}

// Add references to triangles from objects to triangles container

void triangles::AddFromObject(GlObject& obj, V_Triangle& triangles)
{
  if (!obj.active_)
    return;

  auto& vxs = obj.GetCoords();
 
  for (auto& face : obj.faces_)
    if (face.active_)
      triangles.emplace_back(vxs, obj.shading_, face, obj.textures_);
}

// Add references to triangles from objects to triangles container

void triangles::AddFromObjects(V_GlObject& arr, V_Triangle& triangles)
{
  for (auto& obj : arr)
  {
    if (!obj.active_)
      continue;

    auto& vxs = obj.GetCoords();
  
    for (auto& face : obj.faces_)
      if (face.active_)
        triangles.emplace_back(vxs, obj.shading_, face, obj.textures_);
  }
}

void triangles::AddFromTriangles(cV_Triangle& from, V_Triangle& to)
{
  std::copy(from.begin(), from.end(), std::back_inserter(to));
}

// Cull triangles by 6 frustum planes and clip by near_z plane. Function works
// in camera coordinates. In perfomance reasons we should call it before lighting,
// but after removing backfaces
// Culling procedure:
// 1) tests triangles to intersect of 5 planes except near_z. If triangle is
//  out of the one plane, the we cull it. If it inside all 5 planes or
//  intersects one of them, then we don`t cull it (cutting will be in triangles
//  rasterization function)
// 2) test triangles to intersect of near_z plane. If intersects, then clip

int triangles::CullAndClip(V_Triangle& arr, const GlCamera& cam)
{
  // Prepare container for new triangles if we would divide old
  
  V_Triangle new_tris {};
  
  // Test planes

  int total {};
  for (auto& tri : arr)
  {
    // 1. Start culling triangle by L and R planes

    // Compute x-axis projection of point, which lies on L and R plane
    // when z = 1. Attent, that cam.wov_/2 - is proj_x when z = cam.dov_

    if (!tri.active_) 
      continue;

    int out_of_l {0};
    int out_of_r {0};
    float proj_x = cam.wov_ * 0.5f / cam.dov_;  // todo: move out of loop

    // Test left and right plane for all 3 vertexes in triangle

    float max_x = proj_x * tri[0].pos_.z;       // max x when z == pos_.z
    if (tri[0].pos_.x < -max_x) ++out_of_l;
    if (tri[0].pos_.x >  max_x) ++out_of_r;
    
    max_x = proj_x * tri[1].pos_.z;
    if (tri[1].pos_.x < -max_x) ++out_of_l;
    if (tri[1].pos_.x >  max_x) ++out_of_r;

    max_x = proj_x * tri[2].pos_.z;     
    if (tri[2].pos_.x < -max_x) ++out_of_l;
    if (tri[2].pos_.x >  max_x) ++out_of_r;

    // End culling triangle after testing L and R planes

    if (out_of_l == 3 || out_of_r == 3) {
      tri.active_ = false;
      ++total;
    }      

    // 2. Start culling triangle by U and D planes

    if (!tri.active_)
      continue;

    int out_of_u {0};
    int out_of_d {0};
    float proj_y = cam.wov_ * 0.5 / cam.dov_;

    // Test left and right plane for all 3 vertexes in triangle

    float max_y = proj_y * tri[0].pos_.z / cam.ar_;       // max y when z == pos_.z
    if (tri[0].pos_.y < -max_y) ++out_of_d;
    if (tri[0].pos_.y >  max_y) ++out_of_u;
    
    max_y = proj_y * tri[1].pos_.z / cam.ar_;
    if (tri[1].pos_.y < -max_y) ++out_of_d;
    if (tri[1].pos_.y >  max_y) ++out_of_u;
    
    max_y = proj_y * tri[2].pos_.z / cam.ar_;
    if (tri[2].pos_.y < -max_y) ++out_of_d;
    if (tri[2].pos_.y >  max_y) ++out_of_u;

    // End culling triangle after testing U and D planes

    if (out_of_u == 3 || out_of_d == 3) {
      tri.active_ = false;
      ++total;
    }

    // 3. Start culling triangle by Near_Z and Far_Z planes

    if (!tri.active_)
      continue;

    int out_of_fz {0};
    std::vector<int> behind_nz {};
    behind_nz.reserve(3);
    behind_nz.resize(0);

    // Test near_z and far_z for all 3 vertexes in triangle

    if (tri[0].pos_.z >  cam.z_far_)  ++out_of_fz;
    if (tri[0].pos_.z <= cam.z_near_)
      behind_nz.push_back(0);

    if (tri[1].pos_.z >  cam.z_far_)  ++out_of_fz;
    if (tri[1].pos_.z <= cam.z_near_)
      behind_nz.push_back(1);

    if (tri[2].pos_.z >  cam.z_far_)  ++out_of_fz;
    if (tri[2].pos_.z <= cam.z_near_)
      behind_nz.push_back(2);

    // End culling triangle after testing Near_Z Far_Z planes

    if (out_of_fz == 3 || behind_nz.size() == 3) {
      tri.active_ = false;
      ++total;
    }

    // 4. Start clipping triangle by Near_Z plane
    
    if (!tri.active_)
      continue;

    // Light case (search intersect of 2 lines with Near_Z and make new triangle)

    if (behind_nz.size() == 2)
    {
      // Define points before near_z and point after near_z
      
      int pb1 = behind_nz[0];
      int pb2 = behind_nz[1];
      int pa {0};
      if (pb1+pb2 == 1) 
        pa = 2;
      else if (pb1+pb2 == 2)
        pa = 1;

      if (tri[pb1].pos_.x > tri[pb2].pos_.x)
        std::swap(pb1, pb2);
 
      // Compute intersection

      Parmline3d line_1  {tri[pa].pos_, tri[pb1].pos_};
      Parmline3d line_2  {tri[pa].pos_, tri[pb2].pos_};
      Plane3d    plane_z {0.0f, 0.0f, 1.0f, -cam.z_near_};
      Vector q1 = parmline3d::Intersects(line_1, plane_z);
      Vector q2 = parmline3d::Intersects(line_2, plane_z);

      // Compute color and texture coords for new vertices

      tri.normal_ = 
        vector::CrossProduct(tri[1].pos_-tri[0].pos_, tri[2].pos_-tri[0].pos_);

      float t1 = parmline3d::FindIntersectsT(line_1, plane_z);
      float t2 = parmline3d::FindIntersectsT(line_2, plane_z);

      tri[pb1].pos_ = q1;
      tri[pb2].pos_ = q2;

      FColor dx_cl = tri[pb1].color_ - tri[pa].color_;
      FColor dx_cr = tri[pb2].color_ - tri[pa].color_;
      tri[pb1].color_ = tri[pa].color_ + (dx_cl * t1);
      tri[pb2].color_ = tri[pa].color_ + (dx_cr * t2);
      
      Vector dx_nl = tri[pb1].normal_ - tri[pa].normal_;
      Vector dx_nr = tri[pb2].normal_ - tri[pa].normal_;
      tri[pb1].normal_ = tri[pa].normal_ + (dx_nl * t1);
      tri[pb2].normal_ = tri[pa].normal_ + (dx_nr * t2);

      tri[pb1].texture_ = 
        tri[pa].texture_ + (tri[pb1].texture_ - tri[pa].texture_) * t1;
      tri[pb2].texture_ = 
        tri[pa].texture_ + (tri[pb2].texture_ - tri[pa].texture_) * t2;
    }
    
    // Difficult case

    else if (behind_nz.size() == 1)
    {
      // Define points before near_z and point after near_z
      
      int pb = behind_nz[0];
      int pa1 {0};
      int pa2 {0};
      if (pb == 0) {
        pa1 = 1;
        pa2 = 2;
      }
      else if (pb == 1) {
        pa1 = 0;
        pa2 = 2;
      }
      else {
        pa1 = 0;
        pa2 = 1;
      }

      // Make pa1 the most left index

      if (tri[pa1].pos_.x > tri[pa2].pos_.x)
        std::swap(pa1, pa2);
      
      // Compute intersection

      Parmline3d line_1  {tri[pa1].pos_, tri[pb].pos_};
      Parmline3d line_2  {tri[pa2].pos_, tri[pb].pos_};
      Plane3d    plane_z {0.0f, 0.0f, 1.0f, -cam.z_near_};
      Vector q1 = parmline3d::Intersects(line_1, plane_z);
      Vector q2 = parmline3d::Intersects(line_2, plane_z);

      // Compute new vertices for both triangles (new and old) 

      Vertex v1 {tri[pa2]};
      Vertex v2 {q2};
      Vertex v3 {q1};

      // Compute color and texture coords for new vertices

      float t1 = parmline3d::FindIntersectsT(line_1, plane_z);
      float t2 = parmline3d::FindIntersectsT(line_2, plane_z);

      FColor dx_cl = tri[pb].color_ - tri[pa1].color_;
      FColor dx_cr = tri[pb].color_ - tri[pa2].color_;
      v3.color_ = tri[pa1].color_ + (dx_cl * t1);
      v2.color_ = tri[pa2].color_ + (dx_cr * t2);

      Vector dx_nl = tri[pb].normal_ - tri[pa1].normal_;
      Vector dx_nr = tri[pb].normal_ - tri[pa2].normal_;
      v3.normal_ = tri[pa1].normal_ + (dx_nl * t1);
      v3.normal_ = tri[pa2].normal_ + (dx_nr * t2);

      v3.texture_ = 
        tri[pa1].texture_ + (tri[pb].texture_ - tri[pa1].texture_) * t1;
      v2.texture_ = 
        tri[pa2].texture_ + (tri[pb].texture_ - tri[pa2].texture_) * t2;

      // Make new triangle

      Triangle new_tri {tri};
      new_tri.vxs_ = {v1, v2, v3};
      new_tri.color_ = tri.color_;
      new_tri.normal_ = 
        vector::CrossProduct(v2.pos_-v1.pos_, v3.pos_-v1.pos_);
      new_tris.push_back(new_tri);

      // Change old triangle

      tri.vxs_[pb] = v3;
      // tri.normal_ =
        // vector::CrossProduct(tri[1].pos_-tri[0].pos_, tri[2].pos_-tri[0].pos_);
    }
  }
  
  // Insert back new triangles which was prepared when we divide them above

  std::copy(new_tris.begin(), new_tris.end(), std::back_inserter(arr));
  return total;
}

// Hides invisible faces to viewpoint

int triangles::RemoveHiddenSurfaces(V_Triangle& arr, const GlCamera& cam)
{
  int cnt {0};
  for (auto& tri : arr)
  {
    if (!tri.active_) continue;
    
    // Compute triangle normal
  
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

// Computes triangles normals

void triangles::ComputeNormals(V_Triangle& arr, bool normalize)
{
  for (auto& tri : arr)
  {
    if (!tri.active_)
      continue;

    Vector u {tri[0].pos_, tri[1].pos_};
    Vector v {tri[0].pos_, tri[2].pos_};
    tri.normal_ = vector::CrossProduct(u, v);

    if (normalize && !tri.normal_.IsZero())
      tri.normal_.Normalize();
  }
}

// Apply matrix to all triangles in array

void triangles::ApplyMatrix(const Matrix<4,4>& mx, V_Triangle& arr)
{
  for (auto& tri : arr)
  {
    if (!tri.active_)
      continue;

    tri[0].pos_ = matrix::Multiplie(tri[0].pos_, mx);
    tri[1].pos_ = matrix::Multiplie(tri[1].pos_, mx);
    tri[2].pos_ = matrix::Multiplie(tri[2].pos_, mx);
  }
}

void triangles::World2Camera(
  V_Triangle& arr, const GlCamera& cam, cTrigTable& trig)
{
  auto& cam_dir = cam.dir_;
  auto& cam_pos = cam.vrp_;

  for (auto& tri : arr)
  {
    if (!tri.active_) continue;
    
    float ysin = trig.Sin(-cam_dir.y);
    float ycos = trig.Cos(-cam_dir.y);
    float xsin = trig.Sin(-cam_dir.x);
    float xcos = trig.Cos(-cam_dir.x);
    float zsin = trig.Sin(-cam_dir.z);
    float zcos = trig.Cos(-cam_dir.z);
    
    for (auto& vx : tri.vxs_)
    {
      // Translate position

      vx.pos_ -= cam_pos;

      // Y-axis rotate (yaw)

      if (math::FNotZero(cam_dir.y))
      {
        float vx_old {vx.pos_.x};
        vx.pos_.x = (vx.pos_.x * ycos) + (vx.pos_.z * ysin);
        vx.pos_.z = (vx.pos_.z * ycos) - (vx_old * ysin); 
      }

      // X-axis rotate (pitch)

      if (math::FNotZero(cam_dir.x))
      {
        float vy_old {vx.pos_.y}; 
        vx.pos_.y = (vx.pos_.y * xcos) - (vx.pos_.z * xsin);
        vx.pos_.z = (vx.pos_.z * xcos) + (vy_old * xsin); 
      }

      // Z-axis rotate (roll)

      if (math::FNotZero(cam_dir.z))
      {
        float vx_old {vx.pos_.x};
        vx.pos_.x = (vx.pos_.x * zcos) - (vx.pos_.y * zsin);
        vx.pos_.y = (vx.pos_.y * zcos) + (vx_old * zsin);
      }
    }
  }
}

void triangles::Camera2Persp(V_Triangle& arr, const GlCamera& cam)
{
  for (auto& tri : arr)
  {
    if (!tri.active_) continue;
    
    for (auto& vx : tri.vxs_)
    {
      vx.pos_.x = vx.pos_.x * cam.dov_ / vx.pos_.z;
      vx.pos_.y = vx.pos_.y * cam.dov_ * cam.ar_ / vx.pos_.z;
    }
  }
}

// This function would be used only if we convert camera coords to
// perspective with matrix

void triangles::Homogenous2Normal(V_Triangle& arr)
{
  for (auto& tri : arr)
  {
    if (!tri.active_) continue;
    
    vector::ConvertFromHomogeneous(tri[0].pos_);
    vector::ConvertFromHomogeneous(tri[1].pos_);
    vector::ConvertFromHomogeneous(tri[2].pos_);
  }
}

void triangles::Persp2Screen(V_Triangle& arr, const GlCamera& cam)
{
  for (auto& tri : arr)
  {
    if (!tri.active_) continue;
    
    // Define proportion koefficients

    float kx = cam.scr_w_ / cam.wov_; // how much pix in one unit of proj plane 
    float ky = cam.scr_h_ / cam.wov_;
    
    float half_wov = cam.wov_ / 2;

    // Convert all points from persp to screen

    for (auto& vx : tri.vxs_)
    {
      vx.pos_.x = (vx.pos_.x + half_wov) * kx;   // convert -half_wov +half_wov
      vx.pos_.y = (vx.pos_.y + half_wov) * ky;   // to 0-width, 0-height
    }
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
    return avg_z1 < avg_z2;
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
    return avg_z1 < avg_z2;
  });
}

// The same as above but sorts using near z coordinate

void triangles::SortZNear(V_TrianglePtr& arr)
{
  std::sort(arr.begin(), arr.end(), [](auto* t1, auto* t2)
  {
    auto avg_z1 {std::min(t1->vxs_[0].pos_.z, t1->vxs_[1].pos_.z)};
    avg_z1 = std::min(avg_z1, t1->vxs_[2].pos_.z);
    auto avg_z2 {std::min(t2->vxs_[0].pos_.z, t2->vxs_[1].pos_.z)};
    avg_z2 = std::min(avg_z2, t2->vxs_[2].pos_.z);
    return avg_z1 < avg_z2;
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
    return avg_z1 > avg_z2;
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
    return avg_z1 > avg_z2;
  });
}


} // namespace anshub