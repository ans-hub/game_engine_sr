// *************************************************************
// File:    terrain.cc
// Descr:   represents terrain
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#include "terrain.h"

namespace anshub {

Terrain::Terrain(
  cChar* map_fname, cChar* tex_fname, 
  int div_factor, int obj_width, Shading shading)
  : hm_w_{}
  , hm_h_{}
  , tx_w_{}
  , tx_h_{}
  , obj_w_{obj_width}
  , obj_h_{obj_width}
  , texture_{nullptr}
  , heightmap_{nullptr}
  , vxs_{}
  , chunks_{}
  , shading_{shading}
{
  LoadTexture(tex_fname);
  LoadHeightmap(map_fname);
  ComputeAllVertices(div_factor);
  MakeChunkObjects(obj_width);
}

// Sets shading of terrain

void Terrain::SetShading(Shading s)
{
  shading_ = s;
  for (auto& chunk : chunks_)
    chunk.shading_ = s;
}

// Loads texture of terrain

void Terrain::LoadTexture(const char* fname)
{
  texture_ = std::make_shared<Bitmap>(fname);
  tx_h_ = texture_->height();
  tx_w_ = texture_->width();

  if (tx_h_ == 0 || tx_w_ == 0)
    throw DrawExcept("Texture width or height is zero");

  // Note: unnecessary to check for square of texture or
  // to check is w and h of texture is the factor of two 
  // since we may stretch texture as we wish
}

// Loads heightmap of terrain

void Terrain::LoadHeightmap(const char* fname)
{
  heightmap_ = std::make_unique<Bitmap>(fname);
  hm_h_ = heightmap_->height();
  hm_w_ = heightmap_->width();

  if (hm_h_ == 0 || hm_w_ == 0)
    throw DrawExcept("Heighmap width or height is zero");
  if (tx_h_ != tx_w_)
    throw DrawExcept("Heightmap width and height are not the same");
  if (!math::IsAbsFactorOfTwo(tx_h_) || (!math::IsAbsFactorOfTwo(tx_w_)))
    throw DrawExcept("Heightmap dimensions are not the factor of two");
}

// Compute vertices for most deta;ized level. Divide factor used here
// to regulate impact of pixel color to y coordinate

void Terrain::ComputeAllVertices(int div_factor)
{
  // Helpers computations

  int half_h = hm_h_ / 2;
  int half_w = hm_w_ / 2;
   
  // Compute step of texture for each height map pixel

  float dx_u = (float)(tx_w_ / hm_w_);
  float dx_v = (float)(tx_h_ / hm_h_); 

  // Filling

  vxs_.reserve(hm_h_ * hm_w_);

  for (int z = 0; z < hm_h_; ++z) {
    for (int x = 0; x < hm_w_; ++x) {

      // Make position and texture vectors

      float vx = (float)(x - half_w);
      float vy = (float)(heightmap_->red_channel(x,z) / div_factor);;
      float vz = (float)(-(z - half_h));
      Vector pos {vx, vy, vz};
      Vector tex {x*dx_u, z*dx_v, 0.0f};

      // Fill and place vertex

      Vertex v {};
      v.pos_ = std::move(pos);
      v.texture_ = std::move(tex);
      v.color_ = FColor{color::White};  // by convient in ligthing purposes
      vxs_.emplace_back(v);
    }
  }
}

// Divide vertices into the chunks and makes objects, where chunk_width is
// the count of vertices inside one chunk (should be factor of 2)

void Terrain::MakeChunkObjects(int chunk_width)
{
  if (!math::IsAbsFactorOfTwo(chunk_width))
    throw DrawExcept("Chunk width is not the factor of two");

  int chunks_in_line {hm_w_ / chunk_width};
  int lpitch {chunk_width * chunks_in_line};
  
  // Iterate over objects and create objects array
  // where x and y is position of chunk in chunk 2d square matrix

  for (int y = 0; y < chunks_in_line; ++y) {
    for (int x = 0; x < chunks_in_line; ++x) {
      
      Chunk obj {chunk_width};
      
      obj.active_ = true;
      obj.textured_ = true;
      obj.texture_ = texture_;  // sure, that Bitmap doesn`t copies
      obj.shading_ = shading_;

      int st_ind {y * lpitch * chunk_width + x * chunk_width};
      obj.CopyVertices(vxs_, st_ind, chunk_width, lpitch);
      obj.ComputeDetalizationFaces();

      obj.world_pos_ =
        (obj.vxs_local_.front().pos_ + obj.vxs_local_.back().pos_) / 2;
      obj.world_pos_.y = 0.0f;
      // obj.faces_ = obj.det_faces_.front();
      obj.faces_ = obj.det_faces_[0];
      obj.sphere_rad_ = chunk_width / 2;
      
      chunks_.push_back(obj);
    }
  }
}

// Copies part of vertices from global terrain mesh to the local object
// storage

void Terrain::Chunk::CopyVertices(
  const V_Vertex& vxs, int st_ind, int chunk_width, int lpitch)
{ 
  int end_ind {st_ind + (lpitch * chunk_width)};

  for (int y = st_ind; y < end_ind; y += lpitch)
    for (int x = y; x < y + chunk_width; ++x)
      vxs_local_.push_back(vxs[x]);
}

// Computes all possible detalization faces for object

void Terrain::Chunk::ComputeDetalizationFaces()
{
  det_faces_.resize(0);
  int w = chunk_width_;
  
  // Here we simple use variable `det` as step

  for (int det = 1; det < w; det *= 2){

    V_Face curr_faces {};

    for (int y = 0; y < w-det; y+=det) {    // -det since we take y+det
      for (int x = 0; x < w-det; x+=det) {  // and x+det inside loop
        
        // Create faces

        Face f1 {
          vxs_local_, y*w+x, y*w+x+det, (y+det)*w+x};
        Face f2 {
          vxs_local_, y*w+x+det, (y+det)*w+x+det, (y+det)*w+x};
        
        // Fill face color (by convient this is the white)

        f1.color_ = FColor{color::White};
        f2.color_ = FColor{color::White};

        curr_faces.push_back(f1);
        curr_faces.push_back(f2);
      }
    }
    det_faces_.push_back(curr_faces);
  }
}

}  // namespace anshub