// *************************************************************
// File:    gl_object.h
// Descr:   object (drawable) struct for renderer
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef GC_GL_OBJECT_H
#define GC_GL_OBJECT_H

#include <vector>
#include <array>
#include <fstream>

#include "../math/trig.h"
#include "../math/vector.h"
#include "../math/matrix.h"
#include "../math/segment.h"
#include "../data/ply_loader.h"
#include "fx_colors.h"
#include "gl_triangle.h"
#include "gl_camera_euler.h"
#include "exceptions.h"

namespace anshub {

// Enum to define which coordinates currently used in object

enum class Coords
{
  LOCAL,    // local coordinates
  TRANS     // transformed coordinates
};

// Represents drawable object

struct GlObject
{
  // Aliases

  using Vertexes    = std::vector<Vector>;
  using Edges       = std::vector<std::array<int,3>>;
  using Triangles   = std::vector<Triangle>;
  using Matrix2d    = std::vector<std::vector<double>>;

  // Coordinates data members

  Vertexes  vxs_local_;     // vertexes local coords
  Vertexes  vxs_trans_;     // vertexes current coords
  Coords    current_vxs_;   // chooser between coords type
  Triangles triangles_;     // triangles based on coords above

  // Helper data memebers

  int       id_;            // object id  
  bool      active_;        // object state
  Vector    world_pos_;     // position of obj center in world`s coords
  Vector    v_dir_;         // direction vector
  Vector    v_orient_x_;    // 
  Vector    v_orient_y_;    // orientation vectors  
  Vector    v_orient_z_;    //
  Vector    camera_pos_;    // object pos in camera coordinates
  float     sphere_rad_;    // bounding sphere radius
  
  // Constructors

  GlObject();
  GlObject(const Matrix2d& vxs, const Matrix2d& faces, const Matrix2d& attrs);
  void  SetCoords(Coords c) { current_vxs_ = c; }
  void  CopyCoords(Coords src, Coords dest);
  auto& GetCoords() {
    return (current_vxs_ == Coords::LOCAL) ? vxs_local_ : vxs_trans_; }
  
}; // struct Object

// Helpers to objects

namespace object {

  GlObject  Make(const char*);
  GlObject  Make(const char*, const Vector&, const Vector&);
  void      ResetAttributes(GlObject&);
  bool      Cull(GlObject&, const GlCameraEuler&);
  bool      RemoveHiddenSurfaces(GlObject&, const GlCameraEuler&);
  void      Scale(GlObject&, const Vector&);
  void      SetPosition(GlObject&, const Vector&);
  void      RecalcBoundingRadius(GlObject&);
  void      ApplyMatrix(const Matrix<4,4>&, GlObject&);

} // namespace object

} // namespace anshub

#endif  // GC_GL_OBJECT_H