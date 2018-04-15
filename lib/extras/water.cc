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
  
  this->vxs_local_.emplace_back(Vector{-0.5f, 0.0f, -0.5f});
  this->vxs_local_.emplace_back(Vector{-0.5f, 0.0f, 0.5f});
  this->vxs_local_.emplace_back(Vector{0.5f, 0.0f, 0.5f});
  this->vxs_local_.emplace_back(Vector{0.5f, 0.0f, -0.5f});
  this->faces_.emplace_back(this->vxs_local_, 0, 1, 2);
  this->faces_.emplace_back(this->vxs_local_, 2, 3, 0);
  
  // Colorize faces and vertices

  for (auto& vx : this->vxs_local_) {
    vx.color_ = color_;
    vx.color_.a_ = 0.5f;
  }
  for (auto& face : this->faces_) {
    face.color_ = color_;
    face.color_.a_ = 0.5f;
  }

  // Scale and compute vertices normals

  this->SetCoords(Coords::LOCAL);
  auto factor = static_cast<float>(width_);
  std::cout << factor << '\n'; 
  object::Scale(*this, {factor, factor, factor});
  object::ComputeVertexNormalsV2(*this);

  // Set shading and Y position of water

  this->shading_ = s;
  this->world_pos_.y = height_;
}

}  // namespace anshub
  