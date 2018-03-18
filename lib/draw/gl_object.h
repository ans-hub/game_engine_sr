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
#include <memory>

#include "gl_enums.h"
#include "gl_aliases.h"
#include "gl_camera.h"
#include "fx_colors.h"
#include "gl_coords.h"
#include "gl_face.h"
#include "exceptions.h"
#include "../data/ply_loader.h"
#include "../data/bmp_loader.h"
#include "../system/strings.h"
#include "../math/segment.h"
#include "../math/trig.h"
#include "../math/vector.h"
#include "../math/matrix.h"
#include "../math/matrix_rotate_eul.h"
#include "../math/matrix_camera.h"

namespace anshub {

//***********************************************************************
// DRAWABLE OBJECT DEFINITION
//***********************************************************************

struct GlObject
{
  // Data members: coordinates and colors

  V_Vertex  vxs_local_;     // local vertices
  V_Vertex  vxs_trans_;     // transformed vertices
  Coords    current_vxs_;   // chooser between coords type
  V_Face    faces_;         // faces based on coords above
  psBitmap  texture_;       // texture struct
  bool      textured_;      // is object textured

  // Data members: helpers

  int       id_;            // object id  
  bool      active_;        // object state
  Shading   shading_;       // shading type
  Vector    world_pos_;     // position of obj center in world`s coords
  Vector    v_orient_x_;    // 
  Vector    v_orient_y_;    // orientation vectors  
  Vector    v_orient_z_;    //
  float     sphere_rad_;    // bounding sphere radius
  
  // Constructors

  GlObject();
  GlObject(cMatrix2d& vxs, cMatrix2d& colors, cMatrix2d& faces, cMatrix2d& attrs);
  
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
// Helper functions for ONE OBJECT
//***********************************************************************

namespace object {
  
  // V_GlObject creating

  GlObject  Make(const char*);
  GlObject  Make(
    const char*, TrigTable&, cVector& scale, cVector& pos, cVector& rot);
  GlObject  Make(
    const char*, cVector& scale, cVector& pos);

  // Object attributes manipilation

  bool      Cull(GlObject&, const GlCamera&, const MatrixCamera&);
  bool      Cull(GlObject&, const GlCamera&);  
  int       RemoveHiddenSurfaces(GlObject&, const GlCamera&);
  void      ResetAttributes(GlObject&);
  void      ComputeFaceNormals(GlObject&, bool normalize = true);
  void      ComputeFaceNormalsInv(GlObject&, bool normalize = true);
  void      ComputeVertexNormalsV1(GlObject&);
  void      ComputeVertexNormalsV2(GlObject&);
 
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
  float     ComputeBoundingSphereRadius(V_Vertex& vxs, Axis);

  // Debug purposes

  V_Vertex  ComputeDrawableVxsNormals(const GlObject&, float scale);
  
} // namespace object

//***********************************************************************
// Helper functions for CONTAINER OF OBJECT
//***********************************************************************

namespace objects {

  // V_GlObject attributes manipilation

  int       Cull(V_GlObject&, const GlCamera&, const MatrixCamera&);
  int       Cull(V_GlObject&, const GlCamera&);
  int       RemoveHiddenSurfaces(V_GlObject&, const GlCamera&);  
  void      ResetAttributes(V_GlObject&);
  void      ComputeFaceNormals(V_GlObject&, bool normalize = false);
  void      ComputeVertexNormalsV1(V_GlObject&);
  void      ComputeVertexNormalsV2(V_GlObject&);

  // V_GlObject transformation

  void      Translate(V_GlObject&, const Vector&);
  void      Rotate(V_GlObject&, const Vector&, const TrigTable&);
  void      Rotate(V_GlObject&, const std::vector<Vector>&, const TrigTable&);
  void      ApplyMatrix(const Matrix<4,4>&, V_GlObject&);
  
  // V_GlObject coords helpers

  void      World2Camera(V_GlObject&, const GlCamera&);
  void      Camera2Persp(V_GlObject&, const GlCamera&);
  void      Persp2Screen(V_GlObject&, const GlCamera&);
  void      Homogenous2Normal(V_GlObject&);

  // V_GlObject helpers

  void      SetCoords(V_GlObject&, Coords);
  void      CopyCoords(V_GlObject&, Coords, Coords);
  void      SortZ(V_GlObject&);
  
} // namespace objects

//**************************************************************************
// Inline functions implementation
//**************************************************************************

// Converts homogenous coordinates (w != 1) to normal 3d

inline void object::Homogenous2Normal(GlObject& obj)
{
  auto& vxs = obj.GetCoords();
  for (auto& vx : vxs)
    vector::ConvertFromHomogeneous(vx.pos_);
}

// The same function as above but for array of objects

inline void objects::Homogenous2Normal(V_GlObject& arr)
{
  for (auto& obj : arr)
  {
    auto& vxs = obj.GetCoords();
    for (auto& vx : vxs)
      vector::ConvertFromHomogeneous(vx.pos_);
  }
}

} // namespace anshub

#endif  // GC_GL_OBJECT_H