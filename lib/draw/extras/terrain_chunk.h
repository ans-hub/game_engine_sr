// *************************************************************
// File:    terrain_chunk.h
// Descr:   represents chunk of TerrainMap
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef GL_TERRAIN_CHUNK_H
#define GL_TERRAIN_CHUNK_H

#include "../gl_object.h"

namespace anshub {

// Class represents textured terrain which is generated from highmap
// By default struct generates one full level of detalization. If we
// need some levels of detalization, we may set them after creating 
// terrain object

// Note : div_factor - used to compress

struct TerrainChunk : public GlObject
{
  TerrainChunk(
    const char* map_fname, const char* tex_fname, int div_factor);
  void SetDetalization(
    std::vector<float> det_lenghts, int min_det, int max_det);
  void UseDetalization(const Vector& vrp);

private:
  int hm_w_;                        // heightmap width
  int hm_h_;                        // heightmap height
  int tx_w_;                        // texture width
  int tx_h_;                        // texture height
  std::vector<float>  det_lengths_; // max lengths for faces detalization
  std::vector<V_Face> det_faces_;   // faces for detalization

  void   ComputeVertices(const Bitmap&, int div_factor);
  V_Face ComputeFaces(int detalization);

}; // struct TerrainChunk

}  // namespace anshub

#endif // GL_TERRAIN_CHUNK_H