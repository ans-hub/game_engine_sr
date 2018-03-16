// *************************************************************
// File:    terrain.cc
// Descr:   generates terrain 
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#include "terrain.h"

namespace anshub {

// Creates GlObject that imitates terrain

Terrain::Terrain(const char* map_fname, const char* tex_fname, int div_factor)
  : GlObject()
  , hm_w_{}
  , hm_h_{}
  , tx_w_{}
  , tx_h_{}
  , det_lengths_{}
  , det_faces_{}
{
  // Load texture of terrain and add texture to the object

  this->textured_ = true;
  this->texture_ = std::make_shared<Bitmap>(tex_fname);
  this->shading_ = Shading::FLAT;
  tx_h_ = this->texture_->height();
  tx_w_ = this->texture_->width();

  // Load heightmap and fill vertices of the object

  Bitmap map {map_fname};
  hm_h_ = map.height();
  hm_w_ = map.width();
  ComputeVertices(map, div_factor);

  // Fill faces of the object

  faces_ = ComputeFaces(1);
  det_faces_.push_back(faces_);
}

void Terrain::ComputeVertices(const Bitmap& map, int div_factor)
{
  // Helpers computations

  int half_h = hm_h_ / 2;
  int half_w = hm_w_ / 2;
   
  // Compute step of texture for each height map pixel

  float dx_u = (float)(tx_w_ / hm_w_);
  float dx_v = (float)(tx_h_ / hm_h_); 

  // Filling

  this->vxs_local_.reserve(hm_h_ * hm_w_);

  for (int z = 0; z < hm_h_; ++z) {
    for (int x = 0; x < hm_w_; ++x) {

      // Make position and texture vectors

      float vx = (float)(x - half_w);
      float vy = (float)(map.red_channel(x,z) / div_factor);;
      float vz = (float)(-(z - half_h));
      Vector pos {vx, vy, vz};
      Vector tex {x*dx_u, z*dx_v, 0.0f};

      // Fill and place vertex

      Vertex v {};
      v.pos_ = std::move(pos);
      v.texture_ = std::move(tex);
      v.color_ = FColor{color::White};  // by convient in ligthing purposes
      this->vxs_local_.emplace_back(v);
    }
  }
}

// Returs faces for vertices in object using detalization factor

V_Face Terrain::ComputeFaces(int det)
{
  std::vector<Face> res {};

  // Here we simple use variable `det` as step

  for (int y = 0; y < hm_h_-det; y+=det) {
    for (int x = 0; x < hm_w_-det; x+=det) {
      
      // Create faces

      Face f1 {
        this->vxs_local_, y*hm_w_+x, y*hm_w_+x+det, (y+det)*hm_w_+x};
      Face f2 {
        this->vxs_local_, y*hm_w_+x+det, (y+det)*hm_w_+x+det, (y+det)*hm_w_+x};
      
      // Fill face color (by convient this is the white)

      f1.color_ = FColor{color::White};
      f2.color_ = FColor{color::White};

      res.push_back(f1);
      res.push_back(f2);
    }
  }
  return res;
}

// Recompute levels of terrain detalization. Levels of detalization interpolated
// from min_det to max_det

void Terrain::SetDetalization(
  std::vector<float> det_lenghts, int min_det, int max_det)
{
  // Store lenghts for detalization

  det_lengths_ = det_lenghts;
  std::sort(det_lengths_.begin(), det_lengths_.end());

  // Clear structure for storing new faces

  det_faces_.resize(0);
  det_faces_.reserve(det_lengths_.size());
  
  // Prepare interpolant

  int curr_det = min_det;
  int dx_det = (max_det - min_det) / det_faces_.capacity();
  
  // Fill faces
  
  for (auto it = det_lengths_.begin(); it < det_lengths_.end(); ++it)
  {
    det_faces_.push_back(ComputeFaces(curr_det));
    curr_det += dx_det;
  }
}

// Use detalization in depends of view reference point of camera

void Terrain::UseDetalization(const Vector& vrp)
{
  Vector dist {world_pos_ - vrp};
  float len = dist.Length();

  int curr_level {0};
  int i {0};
  for (const auto& det : det_lengths_)
  {
    if (det < len)
      curr_level = i;
    ++i;
  }

  faces_ = det_faces_[curr_level];
}

} // namespace anshub