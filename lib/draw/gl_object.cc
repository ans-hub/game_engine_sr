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
  , camera_pos_{}
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
  , camera_pos_{}
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
    object::RecalcBoundingRadius(obj);
    return obj;    
  }
  else {
    auto name     = list_props.begin()->first;    // first list prop in "face" 
    auto faces    = ply.GetList("face", {name});
    auto color    = static_cast<double>(color::White);
    auto attrs    = Vector2d(faces.size(), Vector1d{0, color});
    auto obj      = GlObject(vertexes, faces, attrs);
    object::RecalcBoundingRadius(obj);
    return obj;
  }
}

// Makes object from ply file, then scale it and move to world position

GlObject object::Make(
  const char* str, const Vector& scale, const Vector& pos, const Vector& rot)
{
  auto obj = object::Make(str);
  obj.SetCoords(Coords::LOCAL);
  object::Scale(obj, scale);
  object::Rotate(obj, rot)
  object::SetPosition(obj, pos);
  object::RecalcBoundingRadius(obj);
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
  for (auto& face : obj.triangles_)
  {
    auto p0 = obj.vxs_trans_[face.indicies_[0]];
    auto p1 = obj.vxs_trans_[face.indicies_[1]];
    auto p2 = obj.vxs_trans_[face.indicies_[2]];

    Vector u {p0, p1};
    Vector v {p0, p2};
    Vector n = vector::CrossProduct(u,v);
    Vector c {p0, cam.vrp_};      // this is not work with uvn camera

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

// Scale object

void object::Scale(GlObject& obj, const Vector& scale)
{
  auto& vxs = obj.GetCoords();
  for (auto& vx : vxs)
  {
    vx.x *= scale.x;
    vx.y *= scale.y;
    vx.z *= scale.z;
  }
}

// Rotate object

void object::Rotate(GlObject& obj, const Vector& v)
{
  
}

// Set world position of center of object

void object::SetPosition(GlObject& obj, const Vector& pos)
{
  obj.world_pos_ += pos;
}

// Calcs bounding sphere radius based on local coordinates of object

void object::RecalcBoundingRadius(GlObject& obj)
{
  for (const auto& vx : obj.vxs_local_)
  {
    float curr {};
    curr = std::fabs(vx.x);
    if (curr > obj.sphere_rad_) obj.sphere_rad_ = curr;
    curr = std::fabs(vx.y);
    if (curr > obj.sphere_rad_) obj.sphere_rad_ = curr;
    curr = std::fabs(vx.z);
    if (curr > obj.sphere_rad_) obj.sphere_rad_ = curr;    
  }
}

// Refresh object orientation when rotates. This should be used near
// the apply rotate matrix to all vertexes (or in hand mode)

void object::RefreshOrientation(GlObject& obj, const MatrixRotate& mx)
{
  obj.v_orient_x_ = matrix::Multiplie(obj.v_orient_x_, mx);
  obj.v_orient_y_ = matrix::Multiplie(obj.v_orient_y_, mx);
  obj.v_orient_z_ = matrix::Multiplie(obj.v_orient_z_, mx);
}

} // namespace anshub