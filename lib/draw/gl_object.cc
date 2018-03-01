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
  , faces_{}
  , id_{}
  , active_{true}
  , shading_{Shading::CONST}
  , world_pos_{0.0f, 0.0f, 0.0f}
  , v_orient_x_{1.0f, 0.0f, 0.0f}
  , v_orient_y_{0.0f, 1.0f, 0.0f}
  , v_orient_z_{0.0f, 0.0f, 1.0f}
  , sphere_rad_{0.0f}  
{ }

// Creates object with vertexes, faces and attrs

GlObject::GlObject(
  cMatrix2d& pos, cMatrix2d& colors, cMatrix2d& faces, cMatrix2d& attrs)
  : vxs_local_{}
  , vxs_trans_{}
  , current_vxs_{Coords::LOCAL}  
  , faces_{}
  , id_{}
  , active_{true}
  , shading_{}  
  , world_pos_{0.0f, 0.0f, 0.0f}
  , v_orient_x_{1.0f, 0.0f, 0.0f}
  , v_orient_y_{0.0f, 1.0f, 0.0f}
  , v_orient_z_{0.0f, 0.0f, 1.0f}
  , sphere_rad_{0.0f}
{
  // Fill local vertexes with position and color

  vxs_local_.reserve(pos.size());
  for (std::size_t i = 0; i < pos.size(); ++i)
  {
    vxs_local_.emplace_back(
      pos[i][0], pos[i][1], pos[i][2],
      colors[i][0], colors[i][1], colors[i][2]
    );
  }
  vxs_trans_ = vxs_local_;
  
  // Now we prepare shading of object

  shading_ = static_cast<Shading>(attrs[0][0]);

  // Fill triangles (we suppose that we have correct ply file with
  // custom fields or incorrect with emply fields)

  faces_.reserve(faces.size());
  for (std::size_t i = 0; i < faces.size(); ++i)
  {
    auto f1 = faces[i][0];
    auto f2 = faces[i][1];
    auto f3 = faces[i][2];
    faces_.emplace_back(vxs_local_, f1, f2, f3);
  }

  // Fill face colors

  for (std::size_t i = 0; i < faces.size(); ++i)
  {
    faces_[i].color_ = vxs_local_[faces_[i][0]].color_;
  }

  // Calc bounding sphere radius

  for (const auto& vx : vxs_local_)
  {
    float curr {};
    curr = std::fabs(vx.pos_.x);
    if (curr > sphere_rad_) sphere_rad_ = curr;
    curr = std::fabs(vx.pos_.y);
    if (curr > sphere_rad_) sphere_rad_ = curr;
    curr = std::fabs(vx.pos_.z);
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
    attrs = Vector2d(1, Vector1d{static_cast<int>(Shading::FLAT)});
  
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
    colors = Vector2d(vxs.size(), Vector1d{255.0f, 255.0f, 255.0f});

  // Try to load faces

  if (!ply.IsElementPresent("face"))
    throw DrawExcept("Ply file haven't face element");
 
  ply::Vector2d faces {};

  if (ply.IsListPropertyPresent("face", "vertex_indicies"))
     faces = ply.GetList("face", {"vertex_indicies"});
  else {
    auto name = header["face"].list_props_.begin()->first;
    faces = ply.GetList("face", {name});
  }

  // Create object

  auto obj = GlObject(vxs, colors, faces, attrs);
  obj.sphere_rad_ = object::FindFarthestCoordinate(obj);
  return obj;
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
  for (auto& face : obj.faces_)
  {
    if (!face.active_) 
      face.active_ = true; 
  }
  obj.active_ = true;
}

// Refresh face normals (for lighting purposes we should call this function
// in world coordinates)

void object::ComputeFaceNormals(GlObject& obj)
{
  auto& vxs = obj.GetCoords();

  for (auto& face : obj.faces_)
  {
    auto p1 = vxs[face[0]].pos_;
    auto p2 = vxs[face[1]].pos_;
    auto p3 = vxs[face[2]].pos_;
    Vector u {p1, p2};
    Vector v {p1, p3};
    face.normal_ = vector::CrossProduct(u, v);
  }
}

// Compute vertexes normals, using quick method (sometimes innacurate, since
// doesn`t respects to similar faces, like in the case with cube). Computing
// process: compute non-normalized face normals, lenghts of this normals would
// be a weight factor (since length of face normal is 2x square of triangle). 

void object::ComputeVertexNormalsV1(GlObject& obj)
{
  object::ComputeFaceNormals(obj);

  auto& vxs = obj.GetCoords();
  for (auto& vx : vxs)
    vx.normal_ = {0.0f, 0.0f, 0.0f};
  std::vector<uint> cnt (obj.vxs_trans_.size());

  // Accumulate normals

  for (auto& face : obj.faces_)
  {
    vxs[face[0]].normal_ += face.normal_;
    vxs[face[1]].normal_ += face.normal_;
    vxs[face[2]].normal_ += face.normal_;
    ++cnt[face[0]];
    ++cnt[face[1]];
    ++cnt[face[2]];
  }

  // Normalize

  for (std::size_t i = 0; i < vxs.size(); ++i)
  {
    vxs[i].normal_ /= cnt[i];
    vxs[i].normal_.Normalize();
  }
}

// Compute vertexes normals, using more complex method, where as weighted factor 
// would be used angles between edges of triangle. This method is really low
// perfomance since requires compute acos and lengths vectors

void object::ComputeVertexNormalsV2(GlObject& obj)
{
  object::ComputeFaceNormals(obj);

  auto& vxs = obj.GetCoords();
  for (auto& vx : vxs)
    vx.normal_ = {0.0f, 0.0f, 0.0f};

  // Accumulate normals

  for (auto& face : obj.faces_)
  {
    face.normal_.Normalize();
    vxs[face[0]].normal_ += face.normal_ * face.angles_[0];
    vxs[face[1]].normal_ += face.normal_ * face.angles_[1];
    vxs[face[2]].normal_ += face.normal_ * face.angles_[2];
  }
  for (auto& vx : vxs)
    vx.normal_.Normalize();
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
  
  Vertex    world {obj.world_pos_};
  V_Vertex  v_world_pos {world};
  coords::World2Camera(v_world_pos, cam.vrp_, cam.dir_, cam.trig_);
  Vector    obj_pos {v_world_pos.back().pos_};

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

  for (auto& face : obj.faces_)
  {
    auto p0 = obj.vxs_trans_[face[0]];
    auto p1 = obj.vxs_trans_[face[1]];
    auto p2 = obj.vxs_trans_[face[2]];
    
    Vector u {p0.pos_, p1.pos_};
    Vector v {p0.pos_, p2.pos_};
    Vector n = vector::CrossProduct(u,v);   // normal to u and v
    Vector c {p0.pos_, cam.vrp_};                // view vector
    n.Normalize();
    c.Normalize();
    auto prod = vector::DotProduct(c,n);
    if (math::FlessZero(prod))
    {
      face.active_ = false;
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
    vx.pos_ = matrix::Multiplie(vx.pos_, mx);
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
  auto& vxs = obj.GetCoords();
  coords::Persp2Screen(vxs, cam.wov_, cam.scr_w_, cam.scr_h_);
}

// Scale object and recalc bounding radius

void object::Scale(GlObject& obj, const Vector& scale)
{
  auto& vxs = obj.GetCoords();
  for (auto& vx : vxs)
  {
    vx.pos_.x *= scale.x;
    vx.pos_.y *= scale.y;
    vx.pos_.z *= scale.z;
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
    vx.pos_ += pos;
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
      float vx_old {vx.pos_.x};
      vx.pos_.x = (vx.pos_.x * ycos) + (vx.pos_.z * ysin);
      vx.pos_.z = (vx.pos_.z * ycos) - (vx_old * ysin); 
    }
  }
  if (math::FNotZero(v.x))
  {
    float xsin = t.Sin(v.x);
    float xcos = t.Cos(v.x);
    for (auto& vx : vxs)
    {
      float vy_old {vx.pos_.y}; 
      vx.pos_.y = (vx.pos_.y * xcos) - (vx.pos_.z * xsin);
      vx.pos_.z = (vx.pos_.z * xcos) + (vy_old * xsin); 
    }
  }
  if (math::FNotZero(v.z))
  {
    float zsin = t.Sin(v.z);
    float zcos = t.Cos(v.z);
    for (auto& vx : vxs)
    {
      float vx_old {vx.pos_.x};
      vx.pos_.x = (vx.pos_.x * zcos) - (vx.pos_.y * zsin);
      vx.pos_.y = (vx.pos_.y * zcos) + (vx_old * zsin);
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
    rad = std::max(rad, std::fabs(vx.pos_.x));
    rad = std::max(rad, std::fabs(vx.pos_.y));
    rad = std::max(rad, std::fabs(vx.pos_.z));
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

// Computes drawable vertexes normals in world coordinates relative to vertex
// of object

V_Vertex object::ComputeDrawableVxsNormals(const GlObject& obj, float scale)
{
  V_Vertex norms {};
  auto& vxs = obj.GetCoords();
  for (std::size_t i = 0; i < vxs.size(); ++i)
  {
    Vector end = vxs[i].pos_ + (vxs[i].normal_ * scale);
    norms.emplace_back(end);
  }
  return norms;
}

//*************************************************************************
// OBJECTS HELPERS IMPLEMENTATION
//*************************************************************************

// Refresh face normals (for lighting purposes we should call this function
// in world coordinates)

void objects::ComputeFaceNormals(V_GlObject& arr)
{
  for (auto& obj : arr)
    object::ComputeFaceNormals(obj);
}

// Refresh vertex normals (for lighting purposes we should call this function
// in world coordinates)

void objects::ComputeVertexNormalsV1(V_GlObject& arr)
{
  for (auto& obj : arr)
    object::ComputeVertexNormalsV1(obj);
}

// Similar as above but uses second type of vertex computation function

void objects::ComputeVertexNormalsV2(V_GlObject& arr)
{
  for (auto& obj : arr)
    object::ComputeVertexNormalsV2(obj);
}

// All these functions are the same as in ::object namespace but applies
// changes for each object in container

int objects::Cull(V_GlObject& arr, const GlCamera& cam, const MatrixCamera& mx)
{
  int res {0};
  for (auto& obj : arr)
  {
    if (obj.active_ && object::Cull(obj, cam, mx))
      ++res;
  }
  return res;
}

int objects::Cull(V_GlObject& arr, const GlCamera& cam)
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

int objects::RemoveHiddenSurfaces(V_GlObject& arr, const GlCamera& cam)
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

void objects::Translate(V_GlObject& arr, const Vector& pos)
{
  for (auto& obj : arr)
    object::Translate(obj, pos);
}

// Rotates objects using one rotate vector

void objects::Rotate(V_GlObject& arr, const Vector& v, const TrigTable& trig)
{
  for (auto& obj : arr)
    object::Rotate(obj, v, trig);
}

// Rotates objects using vector for each object

void objects::Rotate(
  V_GlObject& arr, const std::vector<Vector>& rot, const TrigTable& trig)
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

void objects::ApplyMatrix(const Matrix<4,4>& mx, V_GlObject& arr)
{
  for (auto& obj : arr)
    object::ApplyMatrix(mx, obj);
}

void objects::World2Camera(V_GlObject& arr, const GlCamera& cam)
{
  for (auto& obj : arr)
  {
    auto& vxs = obj.GetCoords();
    coords::World2Camera(vxs, cam.vrp_, cam.dir_, cam.trig_);
  }
}

void objects::Camera2Persp(V_GlObject& arr, const GlCamera& cam)
{
  for (auto& obj : arr)
  {
    auto& vxs = obj.GetCoords();
    coords::Camera2Persp(vxs, cam.dov_, cam.ar_);
  }
}

void objects::Persp2Screen(V_GlObject& arr, const GlCamera& cam)
{
  for (auto& obj : arr)
  {
    auto& vxs = obj.GetCoords();
    coords::Persp2Screen(vxs, cam.wov_, cam.scr_w_, cam.scr_h_);
  }
}

// Reset all attributes in each object

void objects::ResetAttributes(V_GlObject& arr)
{
  for (auto& obj : arr)
    object::ResetAttributes(obj);
}

void objects::SetCoords(V_GlObject& arr, Coords c)
{
  for (auto& obj : arr)
    obj.SetCoords(c);
}

void objects::CopyCoords(V_GlObject& arr, Coords src, Coords dest)
{
  for (auto& obj : arr)
  {
    obj.CopyCoords(src, dest);
  }
}

// Simple z sort based on z world coordinate

void objects::SortZ(V_GlObject& arr)
{
  std::sort(arr.begin(), arr.end(), [](const GlObject& a, const GlObject& b)
  {
    return a.world_pos_.z > b.world_pos_.z; 
  });  
}

} // namespace anshub