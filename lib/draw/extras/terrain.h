// *************************************************************
// File:    terrain.h
// Descr:   represents terrain map
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef GL_EXTRAS_TERRAIN_H
#define GL_EXTRAS_TERRAIN_H

#include "../exceptions.h"
#include "../gl_aliases.h"
#include "../gl_object.h"
#include "../gl_camera.h"

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
  
  void  SetShading(Shading);
  void  ProcessDetalization(const GlCamera&);
  V_Chunk&  GetChunks() { return chunks_; }

  // void SetDetalization(std::vector<float> det_lenghts);
  // void UseDetalization(const Vector& vrp);

private:

  void LoadTexture(const char*);
  void LoadHeightmap(const char*);
  void ComputeAllVertices(int div_factor);
  void MakeChunks(int obj_width);
  void MakeChunkFaces();
  void FillChunks();
  void AlignNeighboringChunks(Chunk&);

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
    , min_y_{}
    , max_y_{}
    , left_{-1}
    , right_{-1}
    , top_{-1}
    , bottom_{-1}
    , det_faces_{} { }
  ~Chunk() { }

  bool SetFace(int face_num);
  void CopyCoords(Coords src, Coords dest) override;

  void ComputeAllFaces();
  void AlignCorners();
  void AlignLeftToBiggest();
  void AlignRightToBiggest();
  void AlignTopToBiggest();
  void AlignBottomToBiggest();

  int     chunk_width_;  // how many vertices contains chunk in max det
  float   min_y_;
  float   max_y_;
  int     left_;
  int     right_;
  int     top_;
  int     bottom_;
  VV_Face det_faces_;   // faces for different detalization level

}; // struct Terrain::Chunk

}  // namespace anshub

#endif // GL_EXTRAS_TERRAIN_H