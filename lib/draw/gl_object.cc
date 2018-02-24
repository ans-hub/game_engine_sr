// *************************************************************
// File:    gl_object.cc
// Descr:   object (drawable) struct for renderer
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#include "gl_object.h"

namespace anshub {

// Creates empty object

GlObject::GlObject()
  : vxs_local_{}
  , vxs_trans_{}
  , vxs_normals_{}
  , colors_local_{}
  , colors_trans_{}
  , current_vxs_{Coords::LOCAL}
  , triangles_{}
  , id_{}
  , active_{true}
  , world_pos_{0.0f, 0.0f, 0.0f}
  , v_orient_x_{1.0f, 0.0f, 0.0f}
  , v_orient_y_{0.0f, 1.0f, 0.0f}
  , v_orient_z_{0.0f, 0.0f, 1.0f}
  , sphere_rad_{0.0f}  
{ }

// Creates object with vertexes, faces and attrs

GlObject::GlObject(
  cMatrix2d& vxs, cMatrix2d& colors, cMatrix2d& faces, cMatrix2d& attrs)
  : vxs_local_{}
  , vxs_trans_{}
  , vxs_normals_{}
  , colors_local_{}
  , colors_trans_{}
  , current_vxs_{Coords::LOCAL}  
  , triangles_{}
  , id_{}
  , active_{true}
  , world_pos_{0.0f, 0.0f, 0.0f}
  , v_orient_x_{1.0f, 0.0f, 0.0f}
  , v_orient_y_{0.0f, 1.0f, 0.0f}
  , v_orient_z_{0.0f, 0.0f, 1.0f}
  , sphere_rad_{0.0f}
{
  // Fill local vertexes

  vxs_local_.reserve(vxs.size());
  for (const auto& vx : vxs)
    vxs_local_.emplace_back(vx[0], vx[1], vx[2]);
  vxs_trans_ = vxs_local_;

  // Fill local colors

  colors_local_.reserve(colors.size());
  for (const auto& c : colors)
    colors_local_.emplace_back(c[0], c[1], c[2]);
  colors_trans_ = colors_local_;
  
  // Now we prepare all attributes of object

  unsigned int fattrs {0};
  for (auto& attr : attrs)
    fattrs |= static_cast<unsigned int>(attr[0]);

  // Fill triangles (we suppose that we have correct ply file with
  // custom fields or incorrect with emply fields)

  triangles_.reserve(faces.size());
  for (std::size_t i = 0; i < faces.size(); ++i)
  {
    auto x = faces[i][0];
    auto y = faces[i][1];
    auto z = faces[i][2];
    triangles_.emplace_back(vxs_trans_, colors_trans_, x, y, z, fattrs);
  }

  // Calc bounding sphere radius

  for (const auto& vx : vxs_local_)
  {
    float curr {};
    curr = std::fabs(vx.x);
    if (curr > sphere_rad_) sphere_rad_ = curr;
    curr = std::fabs(vx.y);
    if (curr > sphere_rad_) sphere_rad_ = curr;
    curr = std::fabs(vx.z);
    if (curr > sphere_rad_) sphere_rad_ = curr;    
  }

  // Reserve place for normals

  vxs_normals_.resize(vxs.size());
  // object::RefreshVertexNormals(*this);
}

// Copies internal coordinates from source to destination

void GlObject::CopyCoords(Coords src, Coords dest)
{
  if (src == Coords::LOCAL && dest == Coords::TRANS)
    vxs_trans_ = vxs_local_;
  else if (src == Coords::TRANS && dest == Coords::LOCAL)
    vxs_local_ = vxs_trans_;
  if (src == Coords::LOCAL && dest == Coords::TRANS)
    colors_trans_ = colors_local_;
  else if (src == Coords::TRANS && dest == Coords::LOCAL)
    colors_local_ = colors_trans_;
}

// // Copies internal coordinates from source to destination

// void GlObject::CopyColors(Coords src, Coords dest)
// {

//   using color::operator<<;
//   std::cerr << "1" << '\n';
//   for (const auto& c : colors_local_)
//     std::cerr << c << '\n';
//   std::cerr << "2" << '\n';
//   for (const auto& c : colors_trans_)
//     std::cerr << c << '\n';
// }

//***************************************************************************
// HELPERS IMPLEMENTATION
//***************************************************************************

// Makes object from ply file in two ways:
//  1) custom fields in ply file (with attrs)
//  2) ply file, exported from Blender and contains at least "vertexes" with
//  properties "x", "y", "z" and "face" elements

GlObject object::Make(const char* str)
{
  using namespace ply;

  ply::Loader ply {};
  std::ifstream fss {str};
  ply.Load(fss);
  
  // Try to determine if ply contains element called "globals_ply_v2", which is
  // sign of using custom ply file
  
  auto header     = ply.GetHeader();

  // Try to load attributes of object

  ply::Vector2d attrs {};
  
  if (ply.IsElementPresent("globals_ply_v2"))
  {
    if (ply.IsSinglePropertyPresent("globals_ply_v2", "shading"))
      attrs = ply.GetLine("globals_ply_v2", {"shading"});
  }
  else
    attrs = Vector2d(1, Vector1d{Triangle::FLAT_SHADING});
  
  // Try to load vertex coordinates

  if (!ply.IsElementPresent("vertex"))
    throw DrawExcept("Ply file haven't vertexes element");
  if (!ply::helpers::IsSinglePropertiesPresent(ply, "vertex", {"x", "y", "z"}))
    throw DrawExcept("Ply file haven't properties x, y and z");

  ply::Vector2d vxs = ply.GetLine("vertex", {"x", "y", "z"});

  // Try load vertex colors

  ply::Vector2d colors {};
  if (ply::helpers::IsSinglePropertiesPresent(ply, "vertex", {"red", "green", "blue"}))
    colors = ply.GetLine("vertex", {"red", "green", "blue"});
  else
    colors = Vector2d(vxs.size(), Vector1d{255, 255, 255});

  // Try to load faces

  if (!ply.IsElementPresent("face"))
    throw DrawExcept("Ply file haven't face element");
 
  ply::Vector2d faces {};

  if (ply.IsListPropertyPresent("face", "vertex_indicies"))
     faces = ply.GetList("face", {"vertex_indicies"});
  else {
    auto name   = header["face"].list_props_.begin()->first; 
    faces  = ply.GetList("face", {name});
  }

  // Create object

  auto obj    = GlObject(vxs, colors, faces, attrs);
  obj.sphere_rad_ = object::FindFarthestCoordinate(obj);
  return obj;

  // if (globals != header.end())
  // {
  //   auto vxs    = ply.GetLine("vertex", {"x", "y", "z"});
  //   auto colors = ply.GetLine("vertex", {"red", "green", "blue"});
  //   auto faces  = ply.GetList("face", {"vertex_indicies"});
  //   auto attrs  = ply.GetLine("face", {"attr"});
  //   obj.sphere_rad_ = object::FindFarthestCoordinate(obj);
  //   return obj;    
  // }
  // else
  // {
  //   auto vxs    = ply.GetLine("vertex", {"x", "y", "z"});  
  //   auto colors = Vector2d(vxs.size(), Vector1d{255, 255, 255});
  //   auto attrs  = Vector2d(faces.size(), 
  //     Vector1d{Triangle::VISIBLE | Triangle::FLAT_SHADING});
  //   auto obj    = GlObject(vxs, colors, faces, attrs);
  //   return obj;
  // }
}

// Makes object from ply file, then scale it and move to world position

GlObject object::Make(
  const char* str, TrigTable& t, cVector& scale, cVector& pos, cVector& rot)
{
  auto obj = object::Make(str);
  obj.SetCoords(Coords::LOCAL);
  object::Scale(obj, scale);
  object::Move(obj, pos);
  object::Rotate(obj, rot, t);
  return obj;
}

// Reset attributes of object and triangles (before each frame)

void object::ResetAttributes(GlObject& obj)
{
  for (auto& tri : obj.triangles_)
  {
    if (tri.attrs_ & Triangle::HIDDEN) 
      tri.attrs_ ^= Triangle::HIDDEN; 
  }
  obj.active_ = true;
}

// Refresh face normals (for lighting purposes we should call this function
// in world coordinates)

void object::RefreshFaceNormals(GlObject& obj)
{
  auto& vxs = obj.GetCoords();

  for (auto& tri : obj.triangles_)
  {
    auto p1 = vxs[tri.f1_];
    auto p2 = vxs[tri.f2_];
    auto p3 = vxs[tri.f3_];
    Vector u {p1, p2};
    Vector v {p1, p3};
    tri.face_normal_ = vector::CrossProduct(u, v);
  }
}

// Refresh vertex normals (for lighting purposes we should call this function
// in world coordinates)

void object::RefreshVertexNormals(GlObject& obj)
{
  object::RefreshFaceNormals(obj);
  obj.vxs_normals_.clear();
  obj.vxs_normals_.resize(obj.vxs_trans_.size());
  std::vector<uint> cnt (obj.vxs_trans_.size());

  for (auto& tri : obj.triangles_)
  {
    obj.vxs_normals_[tri.f1_] += tri.face_normal_;
    obj.vxs_normals_[tri.f2_] += tri.face_normal_;
    obj.vxs_normals_[tri.f3_] += tri.face_normal_;
    ++cnt[tri.f1_];
    ++cnt[tri.f2_];
    ++cnt[tri.f3_];
  }
  using vector::operator<<;
  for (std::size_t i = 0; i < obj.vxs_normals_.size(); ++i)
  {
    obj.vxs_normals_[i] /= cnt[i];
    obj.vxs_normals_[i].Normalize();
    // std::cerr << obj.vxs_normals_[i] << '\n';
  }
  // std::cerr << std::endl;
}

// Cull objects in cameras coordinates. Since we work in camera coordinates,
// all objects trivially culled by 6 planes of camera. Now all tricky
// rotation dir of camera is normalized

// Note #1 : culling by x and y planes made by the similarity of triangles,
// i.e. we in the fact make acsonometric transform and solve 2d task

// Note #2 : also we may cull objects in world coordinates and when cam matrix
// is known (we just convert obj.world_pos_ with matrix to camera coordinates)

bool object::Cull(GlObject& obj, const GlCamera& cam, const MatrixCamera& mx)
{
  // Translate world coords to camera for world_pos_ of object. This is necessary
  // to see how object center would seen when camera would be in 0;0;0 and 0 angles
  // (i.e. when all objects would be translated in camera coordinates)
  
  auto obj_pos = matrix::Multiplie(obj.world_pos_, mx);

  // Cull z planes

  if (obj_pos.z - obj.sphere_rad_ < cam.z_near_)
    obj.active_ = false;
  
  if (obj_pos.z + obj.sphere_rad_ > cam.z_far_)
    obj.active_ = false;

  // Cull x planes (project point on the view plane and check)

  float x_lhs = (cam.dov_ * (obj_pos.x + obj.sphere_rad_) / obj_pos.z);
  float x_rhs = (cam.dov_ * (obj_pos.x - obj.sphere_rad_) / obj_pos.z);

  if (x_lhs < -(cam.wov_ / 2))
    obj.active_ = false;
  if (x_rhs >  (cam.wov_ / 2))
    obj.active_ = false;  

  // Cull y planes (project point on the view plane and check)
  //  todo : here I forgot to use ar

  float y_dhs = (cam.dov_ * (obj_pos.y + obj.sphere_rad_) / obj_pos.z);
  float y_uhs = (cam.dov_ * (obj_pos.y - obj.sphere_rad_) / obj_pos.z);

  if (y_dhs < -(cam.wov_ / 2))
    obj.active_ = false;  
  if (y_uhs > (cam.wov_ / 2))
    obj.active_ = false;

  return !obj.active_;
}

// Same as above but not matrixes

bool object::Cull(GlObject& obj, const GlCamera& cam)
{
  // Translate world coords to camera for world_pos_ of object. This is necessary
  // to see how object center would seen when camera would be in 0;0;0 and 0 angles
  // (i.e. when all objects would be translated in camera coordinates)
  
  Vertexes world_pos {obj.world_pos_};
  coords::World2Camera(world_pos, cam.vrp_, cam.dir_, cam.trig_);
  Vector obj_pos {world_pos.back()};

  // Cull z planes

  if (obj_pos.z - obj.sphere_rad_ < cam.z_near_)
    obj.active_ = false;
  
  if (obj_pos.z + obj.sphere_rad_ > cam.z_far_)
    obj.active_ = false;

  // Cull x planes (project point on the view plane and check)

  float x_lhs = (cam.dov_ * (obj_pos.x + obj.sphere_rad_) / obj_pos.z);
  float x_rhs = (cam.dov_ * (obj_pos.x - obj.sphere_rad_) / obj_pos.z);

  if (x_lhs < -(cam.wov_ / 2))
    obj.active_ = false;
  if (x_rhs >  (cam.wov_ / 2))
    obj.active_ = false;  

  // Cull y planes (project point on the view plane and check)
  //  todo : here I forgot to use ar

  float y_dhs = (cam.dov_ * (obj_pos.y + obj.sphere_rad_) / obj_pos.z);
  float y_uhs = (cam.dov_ * (obj_pos.y - obj.sphere_rad_) / obj_pos.z);

  if (y_dhs < -(cam.wov_ / 2))
    obj.active_ = false;  
  if (y_uhs > (cam.wov_ / 2))
    obj.active_ = false;

  return !obj.active_;
}

// Removes hidden surfaces in camera coordinates

// This function may be called between world and camera coordinates,
// or in camera coordinates. The first way is preffered as in this case
// we may remove many triangles from the pipeline.

// Note #1 : this function not make objects non-transparent, its just
// helper to reduce amount of rendered triangles

// Note #2 : triangles vertixes indicies should be placed convientinally.
// I.e. in blender at the triangulation step we may choose the way we
// triangulate the stuff

int object::RemoveHiddenSurfaces(GlObject& obj, const GlCamera& cam)
{
  int cnt {0};
  if (!obj.active_) return cnt;

  for (auto& face : obj.triangles_)
  {
    auto p0 = obj.vxs_trans_[face.f1_];
    auto p1 = obj.vxs_trans_[face.f2_];
    auto p2 = obj.vxs_trans_[face.f3_];
    
    Vector u {p0, p1};
    Vector v {p0, p2};
    Vector n = vector::CrossProduct(u,v);   // normal to u and v
    Vector c {p0, cam.vrp_};                // view vector
    n.Normalize();
    c.Normalize();
    auto prod = vector::DotProduct(c,n);
    if (math::FlessZero(prod))
    {
      face.attrs_ |= Triangle::HIDDEN;
      ++cnt;
    }
  }
  return cnt;
}

// Apply matrix to object

void object::ApplyMatrix(const Matrix<4,4>& mx, GlObject& obj)
{
  auto& vxs = obj.GetCoords();
  for (auto& vx : vxs)
    vx = matrix::Multiplie(vx, mx);
}

void object::World2Camera(GlObject& obj, const GlCamera& cam)
{
  auto& vxs = obj.GetCoords();
  coords::World2Camera(vxs, cam.vrp_, cam.dir_, cam.trig_);
}

void object::Camera2Persp(GlObject& obj, const GlCamera& cam)
{
  auto& vxs = obj.GetCoords();
  coords::Camera2Persp(vxs, cam.dov_, cam.ar_);
}

void object::Persp2Screen(GlObject& obj, const GlCamera& cam)
{
  auto& vxs {obj.GetCoords()};
  coords::Persp2Screen(vxs, cam.wov_, cam.scr_w_, cam.scr_h_);
}

// Scale object and recalc bounding radius

void object::Scale(GlObject& obj, const Vector& scale)
{
  auto& vxs = obj.GetCoords();
  for (auto& vx : vxs)
  {
    vx.x *= scale.x;
    vx.y *= scale.y;
    vx.z *= scale.z;
  }
  obj.sphere_rad_ = object::FindFarthestCoordinate(obj);
}

// Set world position of center of object

void object::Move(GlObject& obj, const Vector& pos)
{
  obj.world_pos_ += pos;
}

// Translates all coordinates of object relative to pos

void object::Translate(GlObject& obj, const Vector& pos)
{
  auto& vxs = obj.GetCoords();
  for (auto& vx : vxs)
    vx += pos;
}

// Rotate object in YXZ sequence by rotating each vector relative to
// the origin

void object::Rotate(GlObject& obj, const Vector& v, const TrigTable& t)
{
  auto& vxs = obj.GetCoords();
  if (math::FNotZero(v.y))
  {
    float ysin = t.Sin(v.y);
    float ycos = t.Cos(v.y);
    for (auto& vx : vxs)
    {
      float vx_old {vx.x};
      vx.x = (vx.x * ycos) + (vx.z * ysin);
      vx.z = (vx.z * ycos) - (vx_old * ysin); 
    }
  }
  if (math::FNotZero(v.x))
  {
    float xsin = t.Sin(v.x);
    float xcos = t.Cos(v.x);
    for (auto& vx : vxs)
    {
      float vy_old {vx.y}; 
      vx.y = (vx.y * xcos) - (vx.z * xsin);
      vx.z = (vx.z * xcos) + (vy_old * xsin); 
    }
  }
  if (math::FNotZero(v.z))
  {
    float zsin = t.Sin(v.z);
    float zcos = t.Cos(v.z);
    for (auto& vx : vxs)
    {
      float vx_old {vx.x};
      vx.x = (vx.x * zcos) - (vx.y * zsin);
      vx.y = (vx.y * zcos) + (vx_old * zsin);
    }
  }
}

// Finds farthest absolute value of coordinate (x,y or z) for bounding
// sphere purposes

float object::FindFarthestCoordinate(const GlObject& obj)
{
  float rad {};
  const auto& vxs = obj.GetCoords();
  for (const auto& vx : vxs)
  {
    rad = std::max(rad, std::fabs(vx.x));
    rad = std::max(rad, std::fabs(vx.y));
    rad = std::max(rad, std::fabs(vx.z));
  }
  return rad;
}

// Refresh object orientation when rotates. This should be used near
// the apply rotate matrix to all vertexes (or in hand mode)

void object::RefreshOrientation(GlObject& obj, const MatrixRotateEul& mx)
{
  obj.v_orient_x_ = matrix::Multiplie(obj.v_orient_x_, mx);
  obj.v_orient_y_ = matrix::Multiplie(obj.v_orient_y_, mx);
  obj.v_orient_z_ = matrix::Multiplie(obj.v_orient_z_, mx);
}

//*************************************************************************
// OBJECTS HELPERS IMPLEMENTATION
//*************************************************************************

// Refresh face normals (for lighting purposes we should call this function
// in world coordinates)

void objects::RefreshFaceNormals(GlObjects& arr)
{
  for (auto& obj : arr)
    object::RefreshFaceNormals(obj);
}

// Refresh vertex normals (for lighting purposes we should call this function
// in world coordinates)

void objects::RefreshVertexNormals(GlObjects& arr)
{
  for (auto& obj : arr)
    object::RefreshVertexNormals(obj);
}

// All these functions are the same as in ::object namespace but applies
// changes for each object in container

int objects::Cull(GlObjects& arr, const GlCamera& cam, const MatrixCamera& mx)
{
  int res {0};
  for (auto& obj : arr)
  {
    if (obj.active_ && object::Cull(obj, cam, mx))
      ++res;
  }
  return res;
}

int objects::Cull(GlObjects& arr, const GlCamera& cam)
{
  int res {0};
  for (auto& obj : arr)
  {
    if (obj.active_ && object::Cull(obj, cam))
      ++res;
  }
  return res;
}

// Removes hidden surfaces in each object

int objects::RemoveHiddenSurfaces(GlObjects& arr, const GlCamera& cam)
{
  int cnt {0};
  for (auto& obj : arr)
  {
    if (obj.active_)
      cnt += object::RemoveHiddenSurfaces(obj, cam);
  }
  return cnt;
}

// Translates all objects by given vector

void objects::Translate(GlObjects& arr, const Vector& pos)
{
  for (auto& obj : arr)
    object::Translate(obj, pos);
}

// Rotates objects using one rotate vector

void objects::Rotate(GlObjects& arr, const Vector& v, const TrigTable& trig)
{
  for (auto& obj : arr)
    object::Rotate(obj, v, trig);
}

// Rotates objects using vector for each object

void objects::Rotate(
  GlObjects& arr, const std::vector<Vector>& rot, const TrigTable& trig)
{
  // todo: add assertion (arr.size() == vecs.size())
  
  for (std::size_t i = 0; i < arr.size(); ++i)
  {
    auto& obj = arr[i];
    auto& vec = rot[i];
    object::Rotate(obj, vec, trig);
  }
}

// Apply givemn matrixes to onbjects

void objects::ApplyMatrix(const Matrix<4,4>& mx, GlObjects& arr)
{
  for (auto& obj : arr)
    object::ApplyMatrix(mx, obj);
}

void objects::World2Camera(GlObjects& arr, const GlCamera& cam)
{
  for (auto& obj : arr)
  {
    auto& vxs = obj.GetCoords();
    coords::World2Camera(vxs, cam.vrp_, cam.dir_, cam.trig_);
  }
}

void objects::Camera2Persp(GlObjects& arr, const GlCamera& cam)
{
  for (auto& obj : arr)
  {
    auto& vxs = obj.GetCoords();
    coords::Camera2Persp(vxs, cam.dov_, cam.ar_);
  }
}

void objects::Persp2Screen(GlObjects& arr, const GlCamera& cam)
{
  for (auto& obj : arr)
  {
    auto& vxs {obj.GetCoords()};
    coords::Persp2Screen(vxs, cam.wov_, cam.scr_w_, cam.scr_h_);
  }
}

// Reset all attributes in each object

void objects::ResetAttributes(GlObjects& arr)
{
  for (auto& obj : arr)
    object::ResetAttributes(obj);
}

void objects::SetCoords(GlObjects& arr, Coords c)
{
  for (auto& obj : arr)
    obj.SetCoords(c);
}

void objects::CopyCoords(GlObjects& arr, Coords src, Coords dest)
{
  for (auto& obj : arr)
  {
    obj.CopyCoords(src, dest);
  }
}

// Simple z sort based on z world coordinate

void objects::SortZ(GlObjects& arr)
{
  std::sort(arr.begin(), arr.end(), [](const GlObject& a, const GlObject& b)
  {
    return a.world_pos_.z > b.world_pos_.z; 
  });  
}

//*************************************************************************
// TRIANGLES HELPERS IMPLEMENTATION
//*************************************************************************

// Makes container of triangles references array. This is simple array of
// references.

TrianglesRef triangles::MakeContainer()
{
  return TrianglesRef {};
}

// Add references to triangles from objects to triangles container

void triangles::AddFromObject(GlObject& obj, TrianglesRef& triangles)
{
  if (obj.active_)
  {
    // Now we place here references to triangles and fill them

    for (auto& tri : obj.triangles_)
    {
      tri.v1_ = obj.vxs_trans_[tri.f1_];
      tri.v2_ = obj.vxs_trans_[tri.f2_];
      tri.v3_ = obj.vxs_trans_[tri.f3_];
      tri.c1_ = obj.colors_trans_[tri.f1_];
      tri.c2_ = obj.colors_trans_[tri.f2_];
      tri.c3_ = obj.colors_trans_[tri.f3_];
      triangles.emplace_back(std::ref(tri));
    }
  }
}

// Add references to triangles from objects to triangles container

void triangles::AddFromObjects(GlObjects& arr, TrianglesRef& triangles)
{
  for (auto& obj : arr)
  {
    if (!obj.active_)
      continue;
    
    // Now we place here references to triangles and fill them

    for (auto& tri : obj.triangles_)
    {
      tri.v1_ = obj.vxs_trans_[tri.f1_];
      tri.v2_ = obj.vxs_trans_[tri.f2_];
      tri.v3_ = obj.vxs_trans_[tri.f3_];
      tri.c1_ = obj.colors_trans_[tri.f1_];
      tri.c2_ = obj.colors_trans_[tri.f2_];
      tri.c3_ = obj.colors_trans_[tri.f3_];
      triangles.emplace_back(std::ref(tri));
    }
  }
}

// Hides invisible faces to viewpoint. Works as the same function in
// ::object namespace

int triangles::RemoveHiddenSurfaces(TrianglesRef& arr, const GlCamera& cam)
{
  int cnt {0};
  for (auto& tri_ref : arr)
  {
    auto& tri = tri_ref.get();
      
    Vector u {tri.v1_, tri.v2_};
    Vector v {tri.v1_, tri.v3_};
    Vector n = vector::CrossProduct(u,v);   // normal to u and v
    Vector c {tri.v1_, cam.vrp_};           // view vector

    auto prod = vector::DotProduct(c,n);
    if (math::FlessZero(prod))
    {
      tri.attrs_ |= Triangle::HIDDEN;
      ++cnt;
    }
  }
  return cnt;  
}

// Reset attributes of all triangles
// todo : test it

void triangles::ResetAttributes(TrianglesRef& arr)
{
  for (auto& tri : arr) {
    if (tri.get().attrs_ & Triangle::HIDDEN) 
      tri.get().attrs_ ^= Triangle::HIDDEN; 
  }
}

// Apply matrix to all triangles in array
// todo : test it

void triangles::ApplyMatrix(const Matrix<4,4>& mx, TrianglesRef& arr)
{
  for (auto& tri_ref : arr)
  {
    auto tri = tri_ref.get();
    auto v1 = matrix::Multiplie(tri.v1_, mx);
    auto v2 = matrix::Multiplie(tri.v2_, mx);
    auto v3 = matrix::Multiplie(tri.v3_, mx);
    tri.v1_ = v1;
    tri.v2_ = v2;
    tri.v3_ = v3;
  }
}

void triangles::World2Camera(TrianglesRef& arr, const GlCamera& cam)
{
  for (auto& tri_ref : arr)
  {
    auto& tri = tri_ref.get();
    Vertexes vxs {tri.v1_, tri.v2_, tri.v3_};
    coords::World2Camera(vxs, cam.vrp_, cam.dir_, cam.trig_);
    tri.v1_ = vxs[0];
    tri.v2_ = vxs[1];
    tri.v3_ = vxs[2];
  }
}

void triangles::Camera2Persp(TrianglesRef& arr, const GlCamera& cam)
{
  for (auto& tri_ref : arr)
  {
    auto& tri = tri_ref.get();
    Vertexes vxs {tri.v1_, tri.v2_, tri.v3_};
    coords::Camera2Persp(vxs, cam.dov_, cam.ar_);
    tri.v1_ = vxs[0];
    tri.v2_ = vxs[1];
    tri.v3_ = vxs[2];
  }
}

void triangles::Homogenous2Normal(TrianglesRef& arr)
{
  for (auto& tri_ref : arr)
  {
    auto& tri = tri_ref.get();
    vector::ConvertFromHomogeneous(tri.v1_);
    vector::ConvertFromHomogeneous(tri.v2_);
    vector::ConvertFromHomogeneous(tri.v3_);
  }
}

void triangles::Persp2Screen(TrianglesRef& arr, const GlCamera& cam)
{
  for (auto& tri_ref : arr)
  {
    auto& tri = tri_ref.get();
    Vertexes vxs {tri.v1_, tri.v2_, tri.v3_};
    coords::Persp2Screen(vxs, cam.wov_, cam.scr_w_, cam.scr_h_);
    tri.v1_ = vxs[0];
    tri.v2_ = vxs[1];
    tri.v3_ = vxs[2];
  }
}

// We should do this before acsonometric projection

void triangles::SortZ(TrianglesRef& arr)
{
  std::sort(arr.begin(), arr.end(), [](auto& t1, auto& t2)
  {
    return t1.get().v1_.z > t2.get().v1_.z;
  });
}

} // namespace anshub