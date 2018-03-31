// *************************************************************
// File:    terrain.h
// Descr:   represents terrain map
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef GL_EXTRAS_TERRAIN_H
#define GL_EXTRAS_TERRAIN_H

#include <memory>

#include "../exceptions.h"
#include "../gl_aliases.h"
#include "../gl_object.h"
#include "../gl_camera.h"
#include "../gl_vertex.h"

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
  
  auto& GetChunks() { return chunks_; }
  void  SetShading(Shading);
  void  SetDetalization(const V_Float&);
  void  ProcessDetalization(const GlCamera&);
  float FindGroundPosition(const GlCamera&);

private:

  void LoadTexture(const char*);
  void LoadHeightmap(const char*);
  void ComputeAllVertices(int div_factor);
  void MakeChunks(int obj_width);
  void MakeChunkFaces();
  void FillChunks();
  void ComputeVerticesNormals();

  int       hm_w_;                  // heightmap width
  int       hm_h_;                  // heightmap height
  int       tx_w_;                  // texture width
  int       tx_h_;                  // texture height
  int       obj_w_;                 // object array width (how many vertices in width)
  int       obj_h_;                 // object array height
  P_Bitmap texture_;
  Bitmap    heightmap_;
  V_Vertex  vxs_;                   // vertices for all mesh
  V_Chunk   chunks_;                // chunks of terrain
  Shading   shading_;
  V_Float   distances_;             // distances to determine detalization level

}; // struct Terrain

// Nested class of Terrain. Represents chunk of Terrain

struct Terrain::Chunk : GlObject
{
  Chunk(const V_Vertex& vxs, int ln, int rn, int tn, int bn);
  ~Chunk() { }

  bool SetFace(int face_num);
  int  DetLevels() const { return det_faces_.size(); }
  void CopyCoords(Coords src, Coords dest) override;
  void ComputeAllFaces();
  void AlignNeighboringChunks(std::vector<Chunk>&);

private:
  void AlignLeftSide(float step);
  void AlignRightSide(float step);
  void AlignTopSide(float step);
  void AlignBottomSide(float step);
  void RecoverLeftSide();
  void RecoverRightSide();
  void RecoverTopSide();
  void RecoverBottomSide();

  V_Vertex  vxs_backup_;    // backup of local vertices
  VV_Face   det_faces_;     // faces for different detalization levels
  int       vxs_step_;      // step between vertices
  int       vxs_in_row_;    // how many vertices contains chunk in max det
  int       left_chunk_;    // index of left neighboring chunk
  int       right_chunk_;   // index of right neighboring chunk
  int       top_chunk_;     // index of top neighboring chunk
  int       bottom_chunk_;  // index of bottom neighboring chunk
  float     min_y_;         // min y coordinate  
  float     max_y_;         // max y coordinate

}; // struct Terrain::Chunk

}  // namespace anshub

#endif // GL_EXTRAS_TERRAIN_H