// *************************************************************
// File:    nature.cc
// Descr:   represents nature of the terrain
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#include "nature.h"

namespace anshub {

// Load map of nature objects

Nature::Nature(
  const std::string& map_fname, const Terrain& terrain, TrigTable& trig)
  : map_{map_fname.c_str()}
  , terrain_{terrain}
  , objects_{}
  , fnames_{}
  , trig_{trig}
{
  rand_toolkit::start_rand();
}

// Recognize objects in loaded map and place them into container

void Nature::RecognizeObjects()
{
  int half_hw = (map_.width() - 1 ) / 2;

  // Iterate over all object map and set coords as we set it in height map

  for (uint y = 0; y < map_.height(); ++y)
  {
    for (uint x = 0; x < map_.width(); ++x)
    {
      // Pick each pixel color from bitmap and convert into fname

      Color<uchar> marker {};
      map_.get_pixel(x, y, marker.r_, marker.g_, marker.b_);
      auto obj_fname = fnames_[marker.GetARGB()];

      // If pixel color is recognized and have respective object fname

      if (!obj_fname.empty())
      {
        // Create object

        auto obj = object::Make(obj_fname.c_str());
        obj.world_pos_.x = (int)x - half_hw;
        obj.world_pos_.z = -((int)y - half_hw);
        obj.world_pos_.y = terrain_.FindGroundPosition(obj.world_pos_);
        obj.SetCoords(Coords::LOCAL);
        
        // Initial scale

        float scale {rand_toolkit::get_rand(1.0f, 1.7f)};   // todo: !!! bad
        object::Scale(obj, {scale, scale, scale});

        // Initial rotate

        float roll {rand_toolkit::get_rand(0.0f, 15.0f)};   // todo: !!! bad
        object::Rotate(obj, {0.0f, roll, 0.0f}, trig_);
        
        objects_.push_back(obj);
      }
    }
  }
}


}  // namespace anshub
  