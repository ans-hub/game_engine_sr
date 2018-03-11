//***********************************************************************
// File:    gl_triangle.h
// Descr:   drawable triangle struct for renderer
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
//***********************************************************************

#ifndef GC_GL_TRIANGLE_H
#define GC_GL_TRIANGLE_H

#include "gl_enums.h"
#include "gl_aliases.h"
#include "fx_colors.h"
#include "gl_camera.h"
#include "gl_face.h"
#include "gl_vertex.h"
#include "gl_object.h"
#include "../data/bmp_loader.h"
#include "../math/vector.h"

namespace anshub {

//***********************************************************************
// Represents separated drawable triangle struct
//***********************************************************************

struct Triangle
{
  Triangle(const V_Vertex& vxs, Shading shading, const Face& f, Bitmap* tex)
  : active_{true}
  , shading_{shading}
  , vxs_{ {
      vxs[f.vxs_[0]], vxs[f.vxs_[1]], vxs[f.vxs_[2]]
    } }
  , normal_{f.normal_}
  , color_{f.color_}
  , texture_{tex} { }

  Vertex& operator[](int f) { return vxs_[f]; }
  cVertex& operator[](int f) const { return vxs_[f]; }

  bool      active_;
  Shading   shading_;
  A3_Vertex vxs_;
  Vector    normal_;
  FColor    color_;
  Bitmap*   texture_;

}; // struct Triangle

//***********************************************************************
// Helper functions for CONTAINER OF TRIANGLES
//***********************************************************************

namespace triangles {

  // Triangles array filling

  V_Triangle MakeContainer();
  void      AddFromObject(GlObject&, V_Triangle&);
  void      AddFromObjects(V_GlObject&, V_Triangle&);

  // Triangles array attributes manipilation
  
  int       RemoveHiddenSurfaces(V_Triangle&, const GlCamera&);  
  void      ResetAttributes(V_Triangle&);

  // Triangles array transformation

  void      ApplyMatrix(const Matrix<4,4>&, V_Triangle&);

  // Triangles coords helpers

  void      World2Camera(V_Triangle&, const GlCamera&);
  void      Camera2Persp(V_Triangle&, const GlCamera&);
  void      Persp2Screen(V_Triangle&, const GlCamera&);
  void      Homogenous2Normal(V_Triangle&);

  // Triangles helpers

  void      SortZAvg(V_Triangle&);
  void      SortZFar(V_Triangle&);
  void      SortZAvgInv(V_Triangle&);
  void      SortZFarInv(V_Triangle&);

} // namespace triangles

} // namespace anshub

#endif  // GC_GL_TRIANGLE_H