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
  , state_{}
  , world_pos_{0.0f, 0.0f, 0.0f}
  , v_dir_{0.0f, 0.0f, 1.0f}        // +z direction
  , v_orient_x_{1.0f, 0.0f, 0.0f}
  , v_orient_y_{0.0f, 1.0f, 0.0f}
  , v_orient_z_{0.0f, 0.0f, 1.0f}
{ }

// Creates object with given vertexes vector and edges array

// Object::Object(const Matrix2d& vxs, const Matrix2d& faces)
GlObject::GlObject(const Matrix2d& vxs, const Matrix2d& faces, const Matrix2d& attrs)
  : vxs_local_{}
  , vxs_trans_{}
  , current_vxs_{Coords::LOCAL}  
  , triangles_{}
  , id_{}
  , state_{}
  , world_pos_{0.0f, 0.0f, 0.0f}
  , v_dir_{0.0f, 0.0f, 1.0f}        // +z direction
  , v_orient_x_{1.0f, 0.0f, 0.0f}
  , v_orient_y_{0.0f, 1.0f, 0.0f}
  , v_orient_z_{0.0f, 0.0f, 1.0f}
{
  // Fill vertexes

  vxs_local_.reserve(vxs.size());
  for (const auto& vx : vxs)
    vxs_local_.emplace_back(vx[0], vx[1], vx[2]);
  vxs_trans_ = vxs_local_;
  
  // Fill triangles
  // First we should understand if we have own attrs or we shoul load regular
  // Add scale, pos and rot at the load 
  triangles_.reserve(faces.size());
  for (std::size_t i = 0; i < faces.size(); ++i)
  {
    unsigned int color = attrs[i][1];
    // unsigned int color = (255<<24)|(255<<16)|(255<<8)|255;
    triangles_.emplace_back(
      // vxs_local_, faces[i][0], faces[i][1], faces[i][2], color  // vxs_curr????
      vxs_trans_, faces[i][0], faces[i][1], faces[i][2], color  // vxs_curr????
    );
  }
}

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

// Makes object from my own custom fields ply file

GlObject object::Make(const char* str)
{
  ply::Loader ply {};
  try {
    std::ifstream fss {str};
    ply.Load(fss);

    auto vertexes   = ply.GetLine("vertex", {"x", "y", "z"});
    auto faces      = ply.GetList("face", {"index"});
    auto faces_add  = ply.GetLine("face", {"attr", "color"});

    return GlObject(vertexes, faces, faces_add);
  }
  catch (const ply::Except& e) {
    throw(e);
  }
}

// Makes object from own custom fields ply file and scale object

GlObject object::Make(const char* str, const Vector& scale, const Vector& pos)
{
  auto obj = object::Make(str);
  obj.SetCoords(Coords::LOCAL);
  object::Scale(obj, scale);
  object::Position(obj, pos);
  return obj;
}

// Apply matrix to object

void object::ApplyMatrix(const Matrix<4,4>& mx, GlObject& obj)
{
  auto& vxs = obj.GetCoords();
  for (auto& vx : vxs)
    vx = matrix::Multiplie(vx, mx);
}

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

void object::Position(GlObject& obj, const Vector& pos)
{
  obj.world_pos_ += pos;
}

} // namespace anshub