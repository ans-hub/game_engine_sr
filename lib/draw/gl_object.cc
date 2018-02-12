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
  const Matrix2d& vxs, const Matrix2d& faces, const Matrix2d& attrs)
  : vxs_local_{}
  , vxs_trans_{}
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
  
  // Fill triangles (we suppose that we have correct ply file with
  // custom fields or incorrect with emply fields)

  triangles_.reserve(faces.size());
  for (std::size_t i = 0; i < faces.size(); ++i)
  {
    unsigned int fattr = attrs[i][0];
    unsigned int color = attrs[i][1];
    auto x = faces[i][0];
    auto y = faces[i][1];
    auto z = faces[i][2];
    triangles_.emplace_back(vxs_trans_, x, y, z, color, fattr);
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
}

// Copies internal coordinates from source to destination

void GlObject::CopyCoords(Coords src, Coords dest)
{
  if (src == Coords::LOCAL && dest == Coords::TRANS)
    vxs_trans_ = vxs_local_;
  else if (src == Coords::TRANS && dest == Coords::LOCAL)
    vxs_local_ = vxs_trans_;
}

//***************************************************************************
// HELPERS IMPLEMENTATION
//***************************************************************************

// Makes object from ply file in two ways:
//  1) custom fields in ply file (attrs and color)
//  2) any ply contains "vertexes" (x,y,z) and "face" elements

GlObject object::Make(const char* str)
{
  using namespace ply;

  ply::Loader ply {};
  std::ifstream fss {str};
  ply.Load(fss);
  
  // We suppose that this ply file contains elements "vertex" and "face"

  auto header     = ply.GetHeader();
  auto vertexes   = ply.GetLine("vertex", {"x", "y", "z"});
  
  // Try to determine if ply contains list property magically called "ind1983"
  
  auto list_props = header["face"].list_props_;   // get list props of face
  auto prop_index = list_props.find("ind1983");   // search "magic" index

  // If this "magic" found we suppose ply format with custom fields
  // Else take first list propery and set default attributes
  
  if (prop_index != list_props.end())
  {
    auto faces    = ply.GetList("face", {"ind1983"});
    auto attrs    = ply.GetLine("face", {"attr", "color"});
    auto obj      = GlObject(vertexes, faces, attrs);
    obj.sphere_rad_ = object::FindFarthestCoordinate(obj);
    return obj;    
  }
  else {
    auto name     = list_props.begin()->first;    // first list prop in "face" 
    auto faces    = ply.GetList("face", {name});
    auto color    = static_cast<double>(color::White);
    auto attrs    = Vector2d(faces.size(), Vector1d{0, color});
    auto obj      = GlObject(vertexes, faces, attrs);
    obj.sphere_rad_ = object::FindFarthestCoordinate(obj);
    return obj;
  }
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

  float x_lhs = (cam.dov_ * obj_pos.x / obj_pos.z) + obj.sphere_rad_;
  float x_rhs = (cam.dov_ * obj_pos.x / obj_pos.z) - obj.sphere_rad_;

  if (x_lhs < -(cam.wov_ / 2))
    obj.active_ = false;
  if (x_rhs >  (cam.wov_ / 2))
    obj.active_ = false;  

  // Cull y planes (project point on the view plane and check)

  float y_dhs = (cam.dov_ * obj_pos.y / obj_pos.z) + obj.sphere_rad_;
  float y_uhs = (cam.dov_ * obj_pos.y / obj_pos.z) - obj.sphere_rad_;

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
    if (face.attrs_ & Triangle::DSIDE)
      continue;

    auto p0 = obj.vxs_trans_[face.indicies_[0]];
    auto p1 = obj.vxs_trans_[face.indicies_[1]];
    auto p2 = obj.vxs_trans_[face.indicies_[2]];
    
    Vector u {p0, p1};
    Vector v {p0, p2};
    Vector n = vector::CrossProduct(u,v);   // normal to u and v
    Vector c {p0, cam.vrp_};                // view vector

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
  if (!math::FNotZero(v.y))
  {
    float ysin = t.Sin(v.y);
    float ycos = t.Cos(v.y);
    for (auto& vx : vxs)
    {
      vx.x = (vx.x * ycos) + (vx.z * ysin);
      vx.z = (vx.z * ycos) - (vx.x * ysin); 
    }
  }
  if (!math::FNotZero(v.x))
  {
    float xsin = t.Sin(v.x);
    float xcos = t.Cos(v.x);
    for (auto& vx : vxs)
    {
      vx.y = (vx.y * xcos) - (vx.z * xsin);
      vx.z = (vx.z * xcos) + (vx.y * xsin); 
    }
  }
  if (!math::FNotZero(v.z))
  {
    float zsin = t.Sin(v.z);
    float zcos = t.Cos(v.z);
    for (auto& vx : vxs)
    {
      vx.x = (vx.x * zcos) - (vx.y * zsin);
      vx.y = (vx.y * zcos) + (vx.x * zsin); 
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

// All these functions are the same as in ::object namespace but applies
// changes for each object in container

int objects::Cull(Objects& arr, const GlCamera& cam, const MatrixCamera& mx)
{
  int res {0};
  for (auto& obj : arr)
  {
    if (obj.active_ && object::Cull(obj, cam, mx))
      ++res;
  }
  return res;
}

int objects::RemoveHiddenSurfaces(Objects& arr, const GlCamera& cam)
{
  int cnt {0};
  for (auto& obj : arr)
  {
    if (obj.active_)
      cnt += object::RemoveHiddenSurfaces(obj, cam);
  }
  return cnt;
}

void objects::ApplyMatrix(const Matrix<4,4>& mx, Objects& arr)
{
  for (auto& obj : arr)
    object::ApplyMatrix(mx, obj);
}

void objects::ResetAttributes(Objects& arr)
{
  for (auto& obj : arr)
    object::ResetAttributes(obj);
}

//*************************************************************************
// TRIANGLES HELPERS IMPLEMENTATION
//*************************************************************************

// Makes triangles array from object

Triangles triangles::MakeFromObject(const GlObject& obj)
{
  auto& vxs = obj.GetCoords();
  Triangles res {};
  for (const auto& tri : obj.triangles_)
  {
    res.emplace_back(
      vxs[tri.indicies_[0]],
      vxs[tri.indicies_[1]],
      vxs[tri.indicies_[2]],
      tri.color_,
      tri.attrs_
    );
  }
  return res;
}

// Add triangles from object to triangles array

void triangles::AddFromObject(const GlObject& obj, Triangles& cont)
{
  auto& vxs = obj.GetCoords();  
  for (const auto& tri : obj.triangles_)
  {
    cont.emplace_back(
      vxs[tri.indicies_[0]],
      vxs[tri.indicies_[1]],
      vxs[tri.indicies_[2]],
      tri.color_,
      tri.attrs_
    );
  }
}

// Cull triangles from triangles array. Wors as the same function in
// ::object namespace

bool triangles::Cull(Triangles&, const GlCamera&, const MatrixCamera&)
{
  // Translate world coords to camera for zero vertex of each triangle. This is
  // necessary to see how triangle vertex would seen when camera would be in 0;0;0
  // and 0 angles (i.e. when all triangles would be translated in camera coordinates)
  
  // auto vx_pos = matrix::Multiplie(obj.world_pos_, mx);

  // // Cull z planes

  // if (obj_pos.z - obj.sphere_rad_ < cam.z_near_)
  //   obj.active_ = false;
  
  // if (obj_pos.z + obj.sphere_rad_ > cam.z_far_)
  //   obj.active_ = false;

  // // Cull x planes (project point on the view plane and check)

  // float x_lhs = (cam.dov_ * obj_pos.x / obj_pos.z) + obj.sphere_rad_;
  // float x_rhs = (cam.dov_ * obj_pos.x / obj_pos.z) - obj.sphere_rad_;

  // if (x_lhs < -(cam.wov_ / 2))
  //   obj.active_ = false;
  // if (x_rhs >  (cam.wov_ / 2))
  //   obj.active_ = false;  

  // // Cull y planes (project point on the view plane and check)

  // float y_dhs = (cam.dov_ * obj_pos.y / obj_pos.z) + obj.sphere_rad_;
  // float y_uhs = (cam.dov_ * obj_pos.y / obj_pos.z) - obj.sphere_rad_;

  // if (y_dhs < -(cam.wov_ / 2))
  //   obj.active_ = false;  
  // if (y_uhs > (cam.wov_ / 2))
  //   obj.active_ = false;

  // return !obj.active_;
}

// Hides invisible faces to viewpoint. Works as the same function in
// ::object namespace

int triangles::RemoveHiddenSurfaces(Triangles& arr, const GlCamera& cam)
{
  int cnt {0};
  for (auto& tri : arr)
  {
    auto p0 = tri.vxs_[0];
    auto p1 = tri.vxs_[1];
    auto p2 = tri.vxs_[2];
    
    Vector u {p0, p1};
    Vector v {p0, p2};
    Vector n = vector::CrossProduct(u,v);   // normal to u and v
    Vector c {p0, cam.vrp_};                // view vector

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

void triangles::ResetAttributes(Triangles& arr)
{
  for (auto& tri : arr) {
    if (tri.attrs_ & Triangle::HIDDEN) 
      tri.attrs_ ^= Triangle::HIDDEN; 
  }
}

// Apply matrix to all triangles in array

void triangles::ApplyMatrix(const Matrix<4,4>& mx, Triangles& arr)
{
  for (auto& tri : arr)
    for (auto& vx : tri.vxs_)
      vx = matrix::Multiplie(vx, mx);
}

} // namespace anshub