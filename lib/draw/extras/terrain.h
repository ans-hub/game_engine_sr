// *************************************************************
// File:    terrain.h
// Descr:   represents terrain map
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef GL_EXTRAS_TERRAIN_H
#define GL_EXTRAS_TERRAIN_H

#include "../exceptions.h"
#include "../gl_object.h"
#include "../gl_aliases.h"

#include "../../math/math.h"

namespace anshub {

// Represents global container of vertices and TerrainChunks (derived from
// GlObject). Every chunk contains fixed part of vertices. recieved
// from this struct, and several faces, that represents levels of
// detalization

// Shorteners: hm - heightmap, tex - texture 

struct Terrain
{
  struct Chunk;
  using  V_Chunk = std::vector<Chunk>;

  Terrain(
    cChar* hm_fname, cChar* tex_fname, int div_factor, int obj_width, Shading);
  
  void      SetShading(Shading);
  V_Chunk&  GetChunks() { return chunks_; }

  // void SetDetalization(
  //   std::vector<float> det_lenghts, int min_det, int max_det);
  // void UseDetalization(const Vector& vrp);

private:

  // Private member functions

  void LoadTexture(const char*);
  void LoadHeightmap(const char*);
  void ComputeAllVertices(int div_factor);
  void MakeChunkObjects(int obj_width);

  // Data members

  int       hm_w_;                  // heightmap width
  int       hm_h_;                  // heightmap height
  int       tx_w_;                  // texture width
  int       tx_h_;                  // texture height
  int       obj_w_;                 // object array width
  int       obj_h_;                 // object array height
  psBitmap  texture_;
  puBitmap  heightmap_;
  V_Vertex  vxs_;                   // vertices for all mesh
  V_Chunk   chunks_;                // objects
  Shading   shading_;

}; // struct Terrain

// Nested class of Terrain. Represents chunk of Terrain

struct Terrain::Chunk : public GlObject
{
  Chunk(int width)
  : GlObject()
  , chunk_width_{width}
  , det_faces_{} { }

  void CopyVertices(const V_Vertex&, int st_ind, int line_width, int lpitch);
  void ComputeDetalizationFaces();

  int     chunk_width_;  // how many vertices contains chunk in max det
  VV_Face det_faces_;   // faces for different detalization level

}; // struct Terrain::Chunk

}  // namespace anshub

#endif // GL_EXTRAS_TERRAIN_H