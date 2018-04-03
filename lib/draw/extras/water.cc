// *************************************************************
// File:    water.cc
// Descr:   represents very simple water
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#include "water.h"

namespace anshub {

Water::Water(int width, float height, cFColor& c, Shading s)
  : GlObject{}
  , width_ {width}
  , height_ {height}
  , color_ {c}
{
  // Make vertices
  
  this->vxs_local_.emplace_back(Vector{-1.0f, 0.0f, -1.0f});
  this->vxs_local_.emplace_back(Vector{-1.0f, 0.0f, 1.0f});
  this->vxs_local_.emplace_back(Vector{1.0f, 0.0f, 1.0f});
  this->vxs_local_.emplace_back(Vector{1.0f, 0.0f, -1.0f});
  this->faces_.emplace_back(this->vxs_local_, 0, 1, 2);
  this->faces_.emplace_back(this->vxs_local_, 2, 3, 0);
  
  // Colorize faces and vertices

  for (auto& vx : this->vxs_local_) {
    vx.color_ = {143.0f, 175.0f, 201.0f};
    vx.color_.a_ = 0.5f;
  }
  for (auto& face : this->faces_) {
    face.color_ = {143.0f, 175.0f, 201.0f};
    face.color_.a_ = 0.5f;
  }

  // Scale and compute vertices normals

  this->SetCoords(Coords::LOCAL);
  object::Scale(*this, {width, width, width});
  object::ComputeVertexNormalsV2(*this);

  // Set shading and Y position of water

  this->shading_ = s;
  this->world_pos_.y = height_;
}

}  // namespace anshub
  