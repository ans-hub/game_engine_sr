//***********************************************************************
// File:    gl_triangle.h
// Descr:   drawable triangle struct for renderer
// Author:  Novoselov Anton @ 2017
//***********************************************************************

#ifndef GC_GL_TRIANGLE_H
#define GC_GL_TRIANGLE_H

#include "lib/render/gl_enums.h"
#include "lib/render/gl_aliases.h"
#include "lib/render/fx_colors.h"
#include "lib/render/gl_face.h"
#include "lib/render/gl_vertex.h"
#include "lib/render/gl_object.h"

#include "lib/render/cameras/gl_camera.h"

#include "lib/data/bmp_loader.h"

#include "lib/math/vector.h"
#include "lib/math/plane3d.h"
#include "lib/math/parmline3d.h"

namespace anshub {

//***********************************************************************
// Represents drawable triangle in engine
//***********************************************************************

struct Triangle
{
  Triangle();
  Triangle(const V_Vertex& vxs, Shading shading, const Face& f, V_Bitmap& tex);

  Vertex& operator[](int f) { return vxs_[f]; }
  cVertex& operator[](int f) const { return vxs_[f]; }

  bool      active_;
  Shading   shading_;
  A3_Vertex vxs_;
  Vector    normal_;
  FColor    color_;
  V_Bitmap* textures_;

}; // struct Triangle

//***********************************************************************
// Helper functions for CONTAINER OF TRIANGLES
//***********************************************************************

// Here we use two conceptual containers:
//  1) array of triangles
//  2) array of triangles pointers
// Due to perfomance some functions use only array of pointers. I.e., we may sort
// only pointers, and we may draw only pointers

namespace triangles {

  // Triangles array filling

  V_Triangle    MakeBaseContainer(int capacity);
  V_TrianglePtr MakePtrsContainer(int capacity);

  void MakePointers(V_Triangle&, V_TrianglePtr&);
  void AddFromObject(GlObject&, V_Triangle&);
  void AddFromObjects(V_GlObject&, V_Triangle&);
  void AddFromTriangles(cV_Triangle&, V_Triangle&);

  // Triangles array attributes manipilation
  
  int  CullAndClip(V_Triangle&, const GlCamera&);
  int  RemoveHiddenSurfaces(V_Triangle&, const GlCamera&);
  void ResetAttributes(V_Triangle&);
  void ComputeNormals(V_Triangle&, bool normalize = true);
  
  // Triangles array transformation

  void ApplyMatrix(const Matrix<4,4>&, V_Triangle&);

  // Triangles coords helpers

  void World2Camera(V_Triangle&, const GlCamera&, const TrigTable&);
  void Camera2Persp(V_Triangle&, const GlCamera&);
  void Persp2Screen(V_Triangle&, const GlCamera&);
  void Homogenous2Normal(V_Triangle&);

  // Triangles helpers

  void SortZAvg(V_TrianglePtr&);
  void SortZFar(V_TrianglePtr&);
  void SortZNear(V_TrianglePtr&);
  void SortZAvgInv(V_TrianglePtr&);
  void SortZFarInv(V_TrianglePtr&);
  void SortZAvgCounting(V_TrianglePtr&, float far_z);

} // namespace triangles

} // namespace anshub

#endif  // GC_GL_TRIANGLE_H