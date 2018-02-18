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
  Colors    colors_local_;  // local vertexes colors
  Colors    colors_trans_;  // transformed vertexes colors
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
  void      Translate(GlObject&, const Vector&);
  void      Rotate(GlObject&, const Vector&, const TrigTable&);
  void      ApplyMatrix(const Matrix<4,4>&, GlObject&);
  
  // Object helpers

  float     FindFarthestCoordinate(const GlObject&);
  void      RefreshOrientation(GlObject&, const MatrixRotateEul&);
  
} // namespace object

//***********************************************************************
// Helper functions for CONTAINER OF OBJECT
//***********************************************************************

namespace objects {

  // Objects attributes manipilation

  int       Cull(Objects&, const GlCamera&, const MatrixCamera&);
  int       RemoveHiddenSurfaces(Objects&, const GlCamera&);  
  void      ResetAttributes(Objects&);

  // Objects transformation

  void      ApplyMatrix(const Matrix<4,4>&, Objects&);
  
} // namespace objects

//***********************************************************************
// Helper functions for CONTAINER OF TRIANGLES
//***********************************************************************

namespace triangles {

  // Triangles creating

  Triangles MakeFromObject(const GlObject&);
  void      AddFromObject(const GlObject&, Triangles&);

  // Triangles attributes manipilation
  
  bool      Cull(Triangles&, const GlCamera&, const MatrixCamera&);
  int       RemoveHiddenSurfaces(Triangles&, const GlCamera&);  
  void      ResetAttributes(Triangles&);

  // Triangles transformation

  void      ApplyMatrix(const Matrix<4,4>&, Triangles&);

} // namespace triangles

} // namespace anshub

#endif  // GC_GL_OBJECT_H