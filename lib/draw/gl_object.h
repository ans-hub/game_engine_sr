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

#include "fx_colors.h"
#include "gl_triangle.h"
#include "gl_camera.h"
#include "exceptions.h"
#include "../data/ply_loader.h"
#include "../math/segment.h"
#include "../math/trig.h"
#include "../math/vector.h"
#include "../math/matrix.h"
#include "../math/matrix_rotate.h"
#include "../math/matrix_camera.h"

namespace anshub {

// Enum to define which coordinates currently used in object

enum class Coords
{
  LOCAL,    // local coordinates
  TRANS     // transformed coordinates
};

//***********************************************************************
// DRAWABLE OBJECT DEFINITION
//***********************************************************************

struct GlObject
{
  using Vertexes    = std::vector<Vector>;
  using Matrix2d    = std::vector<std::vector<double>>;

  // Data members: coordinates

  Vertexes  vxs_local_;     // vertexes local coords
  Vertexes  vxs_trans_;     // vertexes current coords
  Coords    current_vxs_;   // chooser between coords type
  Triangles triangles_;     // triangles based on coords above

  // Data memebers: helpers

  int       id_;            // object id  
  bool      active_;        // object state
  Vector    world_pos_;     // position of obj center in world`s coords
  Vector    v_orient_x_;    // 
  Vector    v_orient_y_;    // orientation vectors  
  Vector    v_orient_z_;    //
  float     sphere_rad_;    // bounding sphere radius
  
  // Constructors

  GlObject();
  GlObject(const Matrix2d& vxs, const Matrix2d& faces, const Matrix2d& attrs);
  
  // Coordinates routines

  void  SetCoords(Coords c) { current_vxs_ = c; }
  void  CopyCoords(Coords src, Coords dest);
  auto& GetCoords();
  auto& GetCoords() const;
  
}; // struct Object

//***********************************************************************
// INLINE IMPLEMENTATION
//***********************************************************************

  inline auto& GlObject::GetCoords() { 
    return (current_vxs_ == Coords::LOCAL) ? vxs_local_ : vxs_trans_;
  }
  
  inline auto& GlObject::GetCoords() const {
    return (current_vxs_ == Coords::LOCAL) ? vxs_local_ : vxs_trans_;
  }

//***********************************************************************
// HELPERS FUNCTIONS DECLARATION
//***********************************************************************

namespace object {

  // Objects creating

  GlObject  Make(const char*);
  GlObject  Make(const char*, TrigTable&, cVector&, cVector&, cVector&);

  // Object attributes manipilation

  bool      Cull(GlObject&, const GlCamera&, const MatrixCamera&);
  int       RemoveHiddenSurfaces(GlObject&, const GlCamera&);
  void      ResetAttributes(GlObject&);
  
  // Object transformation

  void      Scale(GlObject&, const Vector&);
  void      Move(GlObject&, const Vector&);
  void      Rotate(GlObject&, const Vector&, const TrigTable&);
  void      ApplyMatrix(const Matrix<4,4>&, GlObject&);
  
  // Object helpers

  float     FindFarthestCoordinate(const GlObject&);
  void      RefreshOrientation(GlObject&, const MatrixRotate&);

  // Converters

  TriangleFaces ConvertToTriangles(const GlObject&);
  void      AddToTriangles(const GlObject&, TriangleFaces&);

} // namespace object

} // namespace anshub

#endif  // GC_GL_OBJECT_H