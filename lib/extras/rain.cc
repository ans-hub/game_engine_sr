// *************************************************************
// File:    rain.cc
// Descr:   represents rain
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#include "rain.h"

namespace anshub {

// Blob randomly placed in local position relative to some object, where
// 0.0.0 coord is the center. Emplaced means that blob is placed relative
// some object, else blob is not placed

Blob::Blob(float w)
  : GlObject()
  , local_pos_{}
  , emplaced_{false}
{
  // Create blob model

  vxs_local_.emplace_back(Vector(0.4f, 0.2f, -0.20f));
  vxs_local_.emplace_back(Vector(0.4f, 0.2f, -0.21f));
  vxs_local_.emplace_back(Vector(-0.4f, -0.2f, -0.20f));
  vxs_local_.emplace_back(Vector(-0.4f, -0.2f, -0.21f));
  
  for (auto& vx : vxs_local_)
    vx.color_ = color::fWhite;

  // Create faces

  faces_.emplace_back(vxs_local_, 0, 3, 1);
  faces_.emplace_back(vxs_local_, 1, 3, 0);

  for (auto& face : faces_)
    face.color_ = color::fWhite;
 
  // Create local pos

  local_pos_ = Vector{
    rand_toolkit::get_rand(-w/2.0f, +w/2.0f), // 0.0f - is center of obj
    rand_toolkit::get_rand(-w/2.0f, +w/2.0f),
    rand_toolkit::get_rand(-w/2.0f, +w/2.0f)   
  };

  shading_ = Shading::FLAT;
  current_vxs_ = Coords::LOCAL;
  active_ = true;
}

// Creates container of the Blobs

Rain::Rain(int count, float w)
  : count_{count}
  , width_{w}
  , blobs_{}
  , vel_{0.0f, -1.0f, -1.0f}
{
  blobs_.reserve(count);

  while (count)
  {
    blobs_.emplace_back(Blob(width_));
    --count;
  }
}

// Iterate through each blob and emplace them one time, and other time
// add velocity to emplaced blob while it did`t disappear under the ground

void Rain::Process(cVector& obj_pos)
{
  for (auto& blob : blobs_)
  {
    if (!blob.emplaced_)
    {
      blob.world_pos_ = obj_pos + blob.local_pos_;
      blob.emplaced_ = true;
    }
    
    blob.world_pos_ += vel_;
    if (blob.world_pos_.y < 0.0f)
      blob = Blob(width_);
  }
}

} // namespace anshub