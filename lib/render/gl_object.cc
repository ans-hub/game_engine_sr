// *************************************************************
// File:    gl_object.cc
// Descr:   object struct for renderer
// Author:  Novoselov Anton @ 2017
// *************************************************************

#include "gl_object.h"

namespace anshub {

GlObject::GlObject()
  : vxs_local_{}
  , vxs_trans_{}
  , current_vxs_{Coords::LOCAL}
  , faces_{}
  , textures_{}
  , mipmaps_squares_{}
  , active_{true}
  , shading_{Shading::CONST}
  , world_pos_{0.0f, 0.0f, 0.0f}
  , dir_{0.0f, 0.0f, 0.0f}
  , v_orient_x_{1.0f, 0.0f, 0.0f}
  , v_orient_y_{0.0f, 1.0f, 0.0f}
  , v_orient_z_{0.0f, 0.0f, 1.0f}
  , sphere_rad_{0.0f}  
  , aux_flags_{AuxFlags::NONE}
{ }

GlObject::GlObject(const std::string& ply_fname, cVector& world_pos)
  : vxs_local_{}
  , vxs_trans_{}
  , current_vxs_{Coords::LOCAL}  
  , faces_{}
  , textures_{}
  , mipmaps_squares_{}  
  , active_{true}
  , shading_{Shading::CONST}  
  , world_pos_{world_pos}
  , dir_{0.0f, 0.0f, 0.0f}
  , v_orient_x_{1.0f, 0.0f, 0.0f}
  , v_orient_y_{0.0f, 1.0f, 0.0f}
  , v_orient_z_{0.0f, 0.0f, 1.0f}
  , sphere_rad_{0.0f}
  , aux_flags_{AuxFlags::NONE}
{
  // Load data from file and fill object

  auto loader = load_helpers::LoadFile(ply_fname);
  auto coords = load_helpers::LoadVxsCoordinates(loader);
  auto faces  = load_helpers::LoadFaces(loader);
  auto texels = load_helpers::LoadTexCoordinates(loader);
  auto colors = load_helpers::LoadVxsColors(loader);
  auto attrs  = load_helpers::LoadAttributes(loader);
  
  vxs_local_ = load_helpers::MakeVertices(coords, colors);
  faces_ = load_helpers::MakeFaces(vxs_local_, faces);

  // Load textures

  auto tex_fname = str::Replace(ply_fname, ".ply", ".bmp");
  
  if (!texels.empty() && file::IsExists(tex_fname))
  {
    Bitmap texture {tex_fname, attrs.tex_transparency_};
    constexpr float kGamma = 1.02f;
    textures_ = load_helpers::MakeMipmaps(texture, kGamma);
    mipmaps_squares_ = load_helpers::ComputeMipmapSquares(textures_);
    load_helpers::ApplyTexture(vxs_local_, faces_, texels);
  }

  // Finish loading

  vxs_trans_ = vxs_local_;
  shading_ = attrs.shading_;
  sphere_rad_ = object::FindFarthestCoordinate(vxs_local_);

  // todo: sphere_rad_ = object::ComputeBoundingSphereRadius(vxs_local_, Axis::XYZ);
  // todo: + bounding_box
}

// Copies internal coordinates from source to destination

void GlObject::CopyCoords(Coords src, Coords dest)
{
  if (src == Coords::LOCAL && dest == Coords::TRANS)
    vxs_trans_ = vxs_local_;
  else if (src == Coords::TRANS && dest == Coords::LOCAL)
    vxs_local_ = vxs_trans_;
}

// Loads ply file and returns Loader struct

ply::Loader load_helpers::LoadFile(const std::string& fname)
{
  ply::Loader ply {};
  std::ifstream fss {fname};
  ply.Load(fss);
  return ply;
}

// Returns vertices coordinates stored in ply file in section "vertex" and
// named by convient as "x", "y" and "z"

Vector2d load_helpers::LoadVxsCoordinates(ply::Loader& ply)
{
  if (!ply.IsElementPresent("vertex"))
    throw RenderExcept("Ply file haven't vertexes element");
  if (!ply::helpers::IsSinglePropertiesPresent(ply, "vertex", {"x", "y", "z"}))
    throw RenderExcept("Ply file haven't properties x, y and z");

  return ply.GetLine("vertex", {"x", "y", "z"});
}

// Returns faces stored in ply file in section "face" and named by convient as
// "vertices_indicies"

Vector2d load_helpers::LoadFaces(ply::Loader& ply)
{
  if (!ply.IsElementPresent("face"))
    throw RenderExcept("Ply file haven't face element");
  if (!ply.IsListPropertyPresent("face", "vertex_indices"))
    throw RenderExcept("Ply file haven't vertex_indices property");
    
  return ply.GetList("face", {"vertex_indices"});
}

// Returns vertices coordinates stored in ply file in section "vertex" and
// named by convient as "s" and "t"

Vector2d load_helpers::LoadTexCoordinates(ply::Loader& ply)
{
  if (ply::helpers::IsSinglePropertiesPresent(ply, "vertex", {"s", "t"}))
    return ply.GetLine("vertex", {"s", "t"});
  else
    return {};
}

// Returns vertices colors stored in ply file in section "vertex" and named by
// convient as "red", "green" and "blue" 

Vector2d load_helpers::LoadVxsColors(ply::Loader& ply)
{
  if (ply::helpers::IsSinglePropertiesPresent(ply, "vertex", {"red", "green", "blue"}))
    return ply.GetLine("vertex", {"red", "green", "blue"});
  else
    return {};
}

// Returns attributes of object

ObjAttrs load_helpers::LoadAttributes(ply::Loader& ply)
{
  Vector2d arr {};
  ObjAttrs attrs {};
  
  if (ply.IsElementPresent("globals_ply_v2"))
  {
    if (ply.IsSinglePropertyPresent("globals_ply_v2", "shading"))
    {
      auto arr = ply.GetLine("globals_ply_v2", {"shading"});
      attrs.shading_ = static_cast<Shading>(arr[0][0]);
    }
    if (ply::helpers::IsSinglePropertiesPresent(ply, "globals_ply_v2",{"rt", "gt", "bt"}))
    {
      auto arr = ply.GetLine("globals_ply_v2", {"rt", "gt", "bt"});
      attrs.tex_transparency_ = Color<>(arr[0][0], arr[0][1], arr[0][2]);
    }
  }

  return attrs;  
}

// Returns array of vertices by given pure arrays of coordinates and colors

V_Vertex load_helpers::MakeVertices(cVector2d& coords, cVector2d& colors)
{
  if (coords.empty())
    throw RenderExcept("Vertices coords are empty");
  if (!colors.empty() && coords.size() != colors.size())
    throw RenderExcept("Vertices coords and colors are not the same size");

  V_Vertex vxs {};
  vxs.reserve(coords.size());

  if (!colors.empty())
  {
    for (std::size_t i = 0; i < coords.size(); ++i)
    {
      vxs.emplace_back(
        coords[i][0], coords[i][1], coords[i][2],
        colors[i][0], colors[i][1], colors[i][2]
      );
    }
  }
  else
  {
    auto def_color = color::fWhite;
    for (std::size_t i = 0; i < coords.size(); ++i)
    {
      vxs.emplace_back(
        coords[i][0], coords[i][1], coords[i][2],
        def_color.r_, def_color.g_, def_color.b_
      );
    }
  }
  return vxs;
}

// Returns array of faces by give pure array of faces coords

V_Face load_helpers::MakeFaces(V_Vertex& vxs, cVector2d& farr)
{
  if (farr.empty())
    throw RenderExcept("Faces coords are empty");

  V_Face faces {};
  faces.reserve(farr.size());

  // Fill face coordinates

  for (std::size_t i = 0; i < farr.size(); ++i)
    faces.emplace_back(vxs, farr[i][0], farr[i][1], farr[i][2]);

  // Fill face colors

  for (std::size_t i = 0; i < faces.size(); ++i)
    faces[i].color_ = vxs[faces[i][0]].color_;  

  return faces;
}

// Creates mipmaps

V_Bitmap load_helpers::MakeMipmaps(const Bitmap& tex, float gamma)
{
  // Prepare contanainer of mipmaps and make top level texture

  V_Bitmap mipmaps {};
  if (!tex.width() || !tex.height())
    return mipmaps;
  else
    mipmaps.emplace_back(std::make_shared<Bitmap>(tex));

  // Prepare variables

  int bmp_pitch_w = tex.GetRowIncrement();
  int bmp_texel_w = tex.GetBytesPerPixel();
  auto alpha_color = tex.GetAlphaColor();

  // Generate mipmaps for textures which dimension is factor of 2

  if (!math::IsAbsFactorOfTwo(tex.width()) ||
      !math::IsAbsFactorOfTwo(tex.height())
  )
    return mipmaps;

  // Prepare interpolants

  int curr_w = tex.width();
  int curr_h = tex.height();
  int smaller = std::min(tex.width(), tex.height()); // w and h may be not eq
  int levels = std::log2(smaller);
  
  // Create bmp`s for each level and fill them

  for (int i = 0; i < levels - 1; ++i)
  {
    // Interpolate to current level

    curr_w >>= 1;
    curr_h >>= 1;
    smaller >>= 1;
    bmp_pitch_w >>= 1;

    // Add current bmp to array and get pointers

    auto* prev_tex = mipmaps.back().get()->GetPointer();
    mipmaps.emplace_back(std::make_shared<Bitmap>(curr_w, curr_h, alpha_color));
    auto* curr_tex = mipmaps.back().get()->GetPointer();

    // Iterate through curr bmp

    std::vector<Color<>> neigh {};

    for (int y = 0; y < curr_h; ++y)
    {
      for (int x = 0; x < curr_w; ++x)
      { 
        // Take from previous texture 4 neighboring not transparent pixels
        
        neigh.clear();

        int offset = ((y*2) * bmp_pitch_w*2) + ((x*2) * bmp_texel_w);
        Color<> lt {
          prev_tex[offset + 2], prev_tex[offset + 1], prev_tex[offset + 0]};
        Color<> orig {lt};
        if (lt != alpha_color)
          neigh.push_back(std::move(lt));

        offset = ((y*2) * bmp_pitch_w*2) + ((x*2+1) * bmp_texel_w);
        Color<> rt {
          prev_tex[offset + 2], prev_tex[offset + 1], prev_tex[offset + 0]};
        if (rt != alpha_color)
          neigh.push_back(std::move(rt));

        offset = ((y*2+1) * bmp_pitch_w*2) + ((x*2) * bmp_texel_w);
        Color<> lb {
          prev_tex[offset + 2], prev_tex[offset + 1], prev_tex[offset + 0]};
        if (lb != alpha_color)
          neigh.push_back(std::move(lb));

        offset = ((y*2+1) * bmp_pitch_w*2) + ((x*2+1) * bmp_texel_w);
        Color<> rb {
          prev_tex[offset + 2], prev_tex[offset + 1], prev_tex[offset + 0]};
        if (rb != alpha_color)
          neigh.push_back(std::move(rb));

        // Make average color

        FColor total {};
        if (neigh.size() <= 1)    // see note #1 below
        {
          total = color::Convert<uint, float>(alpha_color);
        }
        else
        {
          for (auto& col : neigh)
            total += color::Convert<uint, float>(col);
          total = total / neigh.size();
          total = total * gamma;  // see note #2 below
        }

        // Save new pixel color to the current texture

        offset = y * bmp_pitch_w + x * bmp_texel_w; 
        curr_tex[offset + 2] = std::floor(total.r_);
        curr_tex[offset + 1] = std::floor(total.g_);
        curr_tex[offset + 0] = std::floor(total.b_);
      }
    }
  }

  return mipmaps;
  
  // Note #1: if we see that half of neighboring pixels is alpha, then we make
  // current pixel alpha too
  // Note #2: gamma correction applied to the total color to make mipmaps not
  // so dark due to averaging
}

// Applies texture coordinates to vertices and fills faces by white color (by
// convient) for texture lighting. Coordinates stays unnormalized

void load_helpers::ApplyTexture(V_Vertex& vxs, V_Face& faces, cVector2d& tex_coords)
{
  for (std::size_t i = 0; i < vxs.size(); ++i)
  {
    vxs[i].texture_.x = tex_coords[i][0];
    vxs[i].texture_.y = tex_coords[i][1];
    vxs[i].color_ = FColor{color::White};
  }

  for (auto& face : faces)
    face.color_ = FColor{color::White};
}

// Calculates squares of each mipmap and place them in array for fast
// access in render functions

V_Uint load_helpers::ComputeMipmapSquares(const V_Bitmap& arr)
{
  V_Uint squares {};
  for (auto& bmp : arr)
    squares.push_back(bmp->width() * bmp->height());
  return squares;
}

// Makes object

GlObject object::Make(
  const std::string& fname, const TrigTable& trig,
  cVector& scale, cVector& world_pos, cVector& rotate
)
{
  GlObject obj(fname, world_pos);
  object::Scale(obj, scale);
  object::Rotate(obj, rotate, trig);
  return obj;
}

// Reset attributes of object and triangles (before each frame)

void object::ResetAttributes(GlObject& obj)
{
  for (auto& face : obj.faces_)
  {
    if (!face.active_) 
      face.active_ = true; 
    face.normal_.Zero();
  }
  obj.active_ = true;
  obj.aux_flags_ = AuxFlags::NONE;
}

// Refresh face normals (for lighting purposes we should call this function
// in world coordinates). Normals are not normalized

void object::ComputeFaceNormals(GlObject& obj, bool normalize)
{
  if (!obj.active_) return;
  
  auto& vxs = obj.GetCoords();

  for (auto& face : obj.faces_)
  {
    auto& p1 = vxs[face[0]].pos_;
    auto& p2 = vxs[face[1]].pos_;
    auto& p3 = vxs[face[2]].pos_;
    Vector u {p1, p2};
    Vector v {p1, p3};
    face.normal_ = vector::CrossProduct(u, v);
    if (normalize && !face.normal_.IsZero())
      face.normal_.Normalize();
  }
}

// Compute inverted face normals

void object::ComputeFaceNormalsInv(GlObject& obj, bool normalize)
{
  if (!obj.active_) return;
  
  auto& vxs = obj.GetCoords();

  for (auto& face : obj.faces_)
  {
    auto& p1 = vxs[face[0]].pos_;
    auto& p2 = vxs[face[1]].pos_;
    auto& p3 = vxs[face[2]].pos_;
    Vector u {p1, p2};
    Vector v {p1, p3};
    face.normal_ = vector::CrossProduct(v, u);
    if (normalize && !face.normal_.IsZero())
      face.normal_.Normalize();
  }
}

// Compute vertexes normals, using quick method (sometimes innacurate, since
// doesn`t respects to similar faces, like in the case with cube). Computing
// process: compute non-normalized face normals, lenghts of this normals would
// be a weight factor (since length of face normal is 2x square of triangle). 

// Note: we should compute face NON-normalized normals before calling this
// function

void object::ComputeVertexNormalsV1(GlObject& obj)
{
  if (!obj.active_) return;

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
    if (!vxs[i].normal_.IsZero())
        vxs[i].normal_.Normalize();
  }
}

// Compute vertices normals, using more complex method, where as weighted factor 
// would be used angles between edges of triangle. This method is really low
// perfomance since requires compute acos and lengths vectors

// Note: we should compute face normalized normals before calling this
// function. Also we can use normals that was after hiding surfaces

void object::ComputeVertexNormalsV2(GlObject& obj)
{
  if (!obj.active_) return;

  auto& vxs = obj.GetCoords();
  for (auto& vx : vxs)
    vx.normal_ = {0.0f, 0.0f, 0.0f};

  // Accumulate normals

  for (auto& face : obj.faces_)
  {
    if (!face.normal_.IsZero())
      face.normal_.Normalize();
    vxs[face[0]].normal_ += face.normal_ * face.angles_[0];
    vxs[face[1]].normal_ += face.normal_ * face.angles_[1];
    vxs[face[2]].normal_ += face.normal_ * face.angles_[2];
  }
  for (auto& vx : vxs) {
    if (!vx.normal_.IsZero())
      vx.normal_.Normalize();
  }
}

// Returns true if flag is present in aux_flags_

bool object::GetAuxFlag(GlObject& obj, AuxFlags f)
{
  return (obj.aux_flags_ & f) == f;
}

// Sets flag to the aux_flags

void object::SetAuxFlag(GlObject& obj, AuxFlags f)
{
  obj.aux_flags_ |= f;
}

// Cull objects in cameras coordinates. Since we work in camera coordinates,
// all objects trivially culled by 6 planes of camera. Now all tricky
// rotation dir of camera is normalized

// Note #1 : culling by x and y planes made by the similarity of triangles,
// i.e. we in the fact make acsonometric transform and solve 2d task

// Note #2 : also we may cull objects in world coordinates and when cam matrix
// is known (we just convert obj.world_pos_ with matrix to camera coordinates)

// Note #3 : this is old legacy function. More better is to use new CullX, CullY
// and CullZ

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

// Same as above but not matrices

bool object::Cull(GlObject& obj, const GlCamera& cam, cTrigTable& trig)
{
  if (!obj.active_)
    return false;

  // Translate world coords to camera for world_pos_ of object. This is necessary
  // to see how object center would seen when camera would be in 0;0;0 and 0 angles
  // (i.e. when all objects would be translated in camera coordinates)
  
  Vector obj_pos {obj.world_pos_};
  coords::World2Camera(obj_pos, cam.vrp_, cam.dir_, trig);

  // Cull z planes

  if (obj_pos.z + obj.sphere_rad_ < cam.z_near_)
    obj.active_ = false;
  
  if (obj_pos.z - obj.sphere_rad_ > cam.z_far_)
    obj.active_ = false;

  // Cull x planes (project point on the view plane and check)

  float x_lhs = (cam.dov_ * (obj_pos.x + obj.sphere_rad_) / obj_pos.z);
  float x_rhs = (cam.dov_ * (obj_pos.x - obj.sphere_rad_) / obj_pos.z);

  if (x_lhs < -(cam.wov_ / 2))
    obj.active_ = false;
  if (x_rhs >  (cam.wov_ / 2))
    obj.active_ = false;  

  // Cull y planes (project point on the view plane and check)
  //  todo : need to use ar here (missed occidentialy)

  float y_dhs = (cam.dov_ * (obj_pos.y + obj.sphere_rad_) / obj_pos.z);
  float y_uhs = (cam.dov_ * (obj_pos.y - obj.sphere_rad_) / obj_pos.z);

  if (y_dhs < -(cam.wov_ / 2))
    obj.active_ = false;
  if (y_uhs > (cam.wov_ / 2))
    obj.active_ = false;

  return !obj.active_;
}

// Culls object by left and right X planes of camera frustrum. We take world pos
// of object and convert it to camera coordinates. Then using radius of bounding 
// spere we decide if it visible or not

bool object::CullX(GlObject& obj, const GlCamera& cam, const TrigTable& trig)
{
  if (!obj.active_)
    return false;
    
  // Convert world pos to camera coordinates

  Vector obj_pos {obj.world_pos_};
  coords::World2Camera(obj_pos, cam.vrp_, cam.dir_, trig);

  // Find max_y coordinate when obj_pos_.z

  float proj_x = cam.wov_ * 0.5f / cam.dov_;      // when z == 1
  float max_x = proj_x * obj_pos.z;               // when z == pos_.z

  if (obj_pos.x + obj.sphere_rad_ < -max_x)
    obj.active_ = false;
  if (obj_pos.x - obj.sphere_rad_ >  max_x)
    obj.active_ = false;

  return !obj.active_;
}

// Culls object by top and bottom Y planes of camera frustrum. We take world pos
// of object and convert it to camera coordinates. Then using radius of bounding 
// spere we decide if it visible or not

bool object::CullY(GlObject& obj, const GlCamera& cam, const TrigTable& trig)
{
  if (!obj.active_)
    return false;

  // Convert world pos to camera coordinates

  Vector obj_pos {obj.world_pos_};
  coords::World2Camera(obj_pos, cam.vrp_, cam.dir_, trig);
  
  // Find max_y coordinate when obj_pos_.z

  float proj_y = cam.wov_ * 0.5f / cam.dov_;      // when z == 1
  float max_y = proj_y * obj_pos.z / cam.ar_;     // when z == pos_.z
  
  if (obj_pos.y + obj.sphere_rad_ < -max_y)
    obj.active_ = false;
  if (obj_pos.y - obj.sphere_rad_ >  max_y)
    obj.active_ = false;

  return !obj.active_;
}

// Culls object by near and far Y planes of camera frustrum. We take world pos
// of object and convert it to camera coordinates. Then using radius of bounding 
// spere we decide if it visible or not

bool object::CullZ(GlObject& obj, const GlCamera& cam, const TrigTable& trig)
{
  if (!obj.active_)
    return false;

  // Convert world pos to camera coordinates

  Vector obj_pos {obj.world_pos_};
  coords::World2Camera(obj_pos, cam.vrp_, cam.dir_, trig);

  // Project point on the view plane and check

  if (obj_pos.z + obj.sphere_rad_ < cam.z_near_)
    obj.active_ = false;
  if (obj_pos.z - obj.sphere_rad_ > cam.z_far_)
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

  auto& vxs = obj.GetCoords();

  for (auto& face : obj.faces_)
  {    
    // Compute face normal if it is absent

    if (face.normal_.IsZero())
    {
      Vector u {vxs[face[0]].pos_, vxs[face[1]].pos_};
      Vector v {vxs[face[0]].pos_, vxs[face[2]].pos_};
      face.normal_ = vector::CrossProduct(u,v);
      if (!face.normal_.IsZero())
        face.normal_.Normalize();
    }

    // Compute vector of view (this is just potential view, not fact)

    Vector view {vxs[face[0]].pos_, cam.vrp_};
    if (!view.IsZero())
      view.Normalize();

    // If angle between view vector and normal > 90 deg, then face is invisible

    auto prod = vector::DotProduct(view, face.normal_);
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

// Convert all vertices of object to camera coordinates

void object::World2Camera(GlObject& obj, const GlCamera& cam, cTrigTable& trig)
{
  auto& vxs = obj.GetCoords();
  coords::World2Camera(vxs, cam.vrp_, cam.dir_, trig);
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

// Convert vertices normals to camera coordinates

void object::VerticesNormals2Camera(
  GlObject& obj, const GlCamera& cam, cTrigTable& trig)
{
  auto& vxs = obj.GetCoords();

  for (auto& vx : vxs) {
    coords::RotateYaw(vx.normal_, -cam.dir_.y, trig);
    coords::RotatePitch(vx.normal_, -cam.dir_.x, trig);
    coords::RotateRoll(vx.normal_, -cam.dir_.z, trig);
    // vx.normal_.Normalize();  // todo: hmm not sure need to comment
  }
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
  return FindFarthestCoordinate(obj.GetCoords());
}

// Finds farthest absolute value of coordinate (x,y or z) for bounding
// sphere purposes

float object::FindFarthestCoordinate(cV_Vertex& vxs)
{
  float rad {};
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

// Refresh object orientation when rotates

void object::RefreshOrientationXYZ(
  GlObject& obj, const Vector& dir, TrigTable& trig)
{
  coords::RotatePitch(obj.v_orient_x_, dir.x, trig);
  coords::RotateYaw(obj.v_orient_x_, dir.y, trig);
  coords::RotateRoll(obj.v_orient_x_, dir.z, trig);
  coords::RotatePitch(obj.v_orient_y_, dir.x, trig);
  coords::RotateYaw(obj.v_orient_y_, dir.y, trig);
  coords::RotateRoll(obj.v_orient_y_, dir.z, trig);
  coords::RotatePitch(obj.v_orient_z_, dir.x, trig);
  coords::RotateYaw(obj.v_orient_z_, dir.y, trig);
  coords::RotateRoll(obj.v_orient_z_, dir.z, trig);
}

// Computes bounding sphere radius using 3 dimensions
// todo: incorrect calculation, to rework

float object::ComputeBoundingSphereRadius(V_Vertex& vxs, Axis axis)
{
  float res {};
  
  float curr {};
  if (axis == Axis::XYZ)
  {
    for (const auto& vx : vxs)
    {
      curr = std::fabs(vx.pos_.x);
      if (curr > res) res = curr;
      curr = std::fabs(vx.pos_.y);
      if (curr > res) res = curr;
      curr = std::fabs(vx.pos_.z);
      if (curr > res) res = curr;    
    }
  }
  else if (axis == Axis::XY)
  {
    for (const auto& vx : vxs)
    {
      curr = std::fabs(vx.pos_.x);
      if (curr > res) res = curr;
      curr = std::fabs(vx.pos_.y);
      if (curr > res) res = curr;
    }
  }
  else if (axis == Axis::XZ)
  {
    for (const auto& vx : vxs)
    {
      curr = std::fabs(vx.pos_.x);
      if (curr > res) res = curr;
      curr = std::fabs(vx.pos_.z);
      if (curr > res) res = curr;
    }
  }
  else if (axis == Axis::YZ)
  {
    for (const auto& vx : vxs)
    {
      curr = std::fabs(vx.pos_.y);
      if (curr > res) res = curr;
      curr = std::fabs(vx.pos_.z);
      if (curr > res) res = curr;
    }
  }
  else if (axis == Axis::X)
  {
    for (const auto& vx : vxs)
    {
      curr = std::fabs(vx.pos_.x);
      if (curr > res) res = curr;
    }
  }
  else if (axis == Axis::Y)
  {
    for (const auto& vx : vxs)
    {
      curr = std::fabs(vx.pos_.y);
      if (curr > res) res = curr;
    }
  }
  else if (axis == Axis::Z)
  {
    for (const auto& vx : vxs)
    {
      curr = std::fabs(vx.pos_.z);
      if (curr > res) res = curr;
    }
  }
  return res;
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

// Refresh face normals (for lighting purposes we should call this function
// in world coordinates)

void objects::ComputeFaceNormals(V_GlObject& arr, bool normalize)
{
  for (auto& obj : arr)
    if (obj.active_) object::ComputeFaceNormals(obj, normalize);
}

// Refresh vertex normals (for lighting purposes we should call this function
// in world coordinates)

void objects::ComputeVertexNormalsV1(V_GlObject& arr)
{
  for (auto& obj : arr) 
    if (obj.active_) object::ComputeVertexNormalsV1(obj);
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

int objects::Cull(V_GlObject& arr, const GlCamera& cam, const TrigTable& trig)
{
  int res {0};
  for (auto& obj : arr)
  {
    if (obj.active_ && object::Cull(obj, cam, trig))
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

// Apply given matrices to an objects

void objects::ApplyMatrix(const Matrix<4,4>& mx, V_GlObject& arr)
{
  for (auto& obj : arr)
    object::ApplyMatrix(mx, obj);
}

void objects::World2Camera(
  V_GlObject& arr, const GlCamera& cam, cTrigTable& trig)
{
  for (auto& obj : arr)
  {
    auto& vxs = obj.GetCoords();
    coords::World2Camera(vxs, cam.vrp_, cam.dir_, trig);
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