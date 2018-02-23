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

#include "gl_aliases.h"
#include "gl_triangle.h"
#include "gl_camera.h"
#include "fx_colors.h"
#include "gl_coords.h"
#include "exceptions.h"
#include "../data/ply_loader.h"
#include "../math/segment.h"
#include "../math/trig.h"
#include "../math/vector.h"
#include "../math/matrix.h"
#include "../math/matrix_rotate_eul.h"
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
  // Data members: coordinates and colors

  Vertexes  vxs_local_;     // vertexes local coords
  Vertexes  vxs_trans_;     // vertexes current coords
  Vectors   vxs_normals_;   // vertexes normals
  FColors   colors_local_;  // local vertexes colors
  FColors   colors_trans_;  // transformed vertexes colors
  Coords    current_vxs_;   // chooser between coords type
  Triangles triangles_;     // triangles based on coords above

  // Data members: helpers

  int       id_;            // object id  
  bool      active_;        // object state
  Vector    world_pos_;     // position of obj center in world`s coords
  Vector    v_orient_x_;    // 
  Vector    v_orient_y_;    // orientation vectors  
  Vector    v_orient_z_;    //
  float     sphere_rad_;    // bounding sphere radius
  
  // Constructors

  GlObject();
  GlObject(cMatrix2d& vxs, cMatrix2d& colors, cMatrix2d& faces, cMatrix2d& attrs);
  // GlObject(const GlObject&) =delete;
  
  // Coordinates routines

  void  SetCoords(Coords c) { current_vxs_ = c; }
  void  CopyCoords(Coords src, Coords dest);
  // void  CopyColors(Coords src, Coords dest);
  auto& GetCoords();
  auto& GetCoords() const;
  auto& GetColors();
  auto& GetColors() const;
  
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

  inline auto& GlObject::GetColors() { 
    return (current_vxs_ == Coords::LOCAL) ? colors_local_ : colors_trans_;
  }
  
  inline auto& GlObject::GetColors() const {
    return (current_vxs_ == Coords::LOCAL) ? colors_local_ : colors_trans_;
  }

//***********************************************************************
// Helper functions for ONE OBJECT
//***********************************************************************

namespace object {
  
  // GlObjects creating

  GlObject  Make(const char*);
  GlObject  Make(const char*, TrigTable&, cVector&, cVector&, cVector&);

  // Object attributes manipilation

  bool      Cull(GlObject&, const GlCamera&, const MatrixCamera&);
  bool      Cull(GlObject&, const GlCamera&);  
  int       RemoveHiddenSurfaces(GlObject&, const GlCamera&);
  void      ResetAttributes(GlObject&);
  void      RefreshFaceNormals(GlObject&);
  void      RefreshVertexNormals(GlObject&);
 
  // Object transformation

  void      Scale(GlObject&, const Vector&);
  void      Move(GlObject&, const Vector&);
  void      Translate(GlObject&, const Vector&);
  void      Rotate(GlObject&, const Vector&, const TrigTable&);
  void      ApplyMatrix(const Matrix<4,4>&, GlObject&);

  // Object coords helpers

  void      World2Camera(GlObject&, const GlCamera&);
  void      Camera2Persp(GlObject&, const GlCamera&);
  void      Persp2Screen(GlObject&, const GlCamera&);
  void      Homogenous2Normal(GlObject&);

  // Object helpers

  float     FindFarthestCoordinate(const GlObject&);
  void      RefreshOrientation(GlObject&, const MatrixRotateEul&);
  
} // namespace object

//***********************************************************************
// Helper functions for CONTAINER OF OBJECT
//***********************************************************************

namespace objects {

  // GlObjects attributes manipilation

  int       Cull(GlObjects&, const GlCamera&, const MatrixCamera&);
  int       Cull(GlObjects&, const GlCamera&);
  int       RemoveHiddenSurfaces(GlObjects&, const GlCamera&);  
  void      ResetAttributes(GlObjects&);
  void      RefreshFaceNormals(GlObjects&);
  void      RefreshVertexNormals(GlObjects&);

  // GlObjects transformation

  void      Translate(GlObjects&, const Vector&);
  void      Rotate(GlObjects&, const Vector&, const TrigTable&);
  void      Rotate(GlObjects&, const std::vector<Vector>&, const TrigTable&);
  void      ApplyMatrix(const Matrix<4,4>&, GlObjects&);
  
  // GlObjects coords helpers

  void      World2Camera(GlObjects&, const GlCamera&);
  void      Camera2Persp(GlObjects&, const GlCamera&);
  void      Persp2Screen(GlObjects&, const GlCamera&);
  void      Homogenous2Normal(GlObjects&);

  // GlObjects helpers

  void      SetCoords(GlObjects&, Coords);
  void      CopyCoords(GlObjects&, Coords, Coords);
  void      SortZ(GlObjects&);
  
} // namespace objects

//***********************************************************************
// Helper functions for CONTAINER OF TRIANGLES
//***********************************************************************

namespace triangles {

  // Triangles array filling

  TrianglesRef MakeContainer();
  void      AddFromObject(GlObject&, TrianglesRef&);
  void      AddFromObjects(GlObjects&, TrianglesRef&);

  // Triangles array attributes manipilation
  
  int       RemoveHiddenSurfaces(TrianglesRef&, const GlCamera&);  
  void      ResetAttributes(TrianglesRef&);

  // Triangles array transformation

  void      ApplyMatrix(const Matrix<4,4>&, TrianglesRef&);

  // Triangles coords helpers

  void      World2Camera(TrianglesRef&, const GlCamera&);
  void      Camera2Persp(TrianglesRef&, const GlCamera&);
  void      Persp2Screen(TrianglesRef&, const GlCamera&);
  void      Homogenous2Normal(TrianglesRef&);

  // Triangles helpers

  void      SortZ(TrianglesRef&);

} // namespace triangles

//**************************************************************************
// Inline functions implementation
//**************************************************************************

// Converts homogenous coordinates (w != 1) to normal 3d

inline void object::Homogenous2Normal(GlObject& obj)
{
  auto& vxs = obj.GetCoords();
  for (auto& vx : vxs)
    vector::ConvertFromHomogeneous(vx);
}

// The same function as above but for array of objects

inline void objects::Homogenous2Normal(GlObjects& arr)
{
  for (auto& obj : arr)
  {
    auto& vxs = obj.GetCoords();
    for (auto& vx : vxs)
      vector::ConvertFromHomogeneous(vx);
  }
}

} // namespace anshub

#endif  // GC_GL_OBJECT_H