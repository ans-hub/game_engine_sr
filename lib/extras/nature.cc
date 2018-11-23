// *************************************************************
// File:    nature.cc
// Descr:   represents simple nature on the terrain
// Author:  Novoselov Anton @ 2017
// *************************************************************

#include "nature.h"

namespace anshub {

// Load map of nature objects

Nature::Nature(
  const std::string& map_fname, float scale, const Terrain& terrain, TrigTable& trig)
  : map_{map_fname.c_str()}
  , terrain_{terrain}
  , objects_{}
  , fnames_{}
  , scale_{scale}
  , trig_{trig}
{
  rand_toolkit::start_rand();
}

// Recognize objects from loaded map and place them into the container

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

        Vector wpos {};
        wpos.x = (int)x - half_hw;
        wpos.z = -((int)y - half_hw);
        wpos.y = terrain_.FindGroundPosition(wpos);
        
        GlObject obj {obj_fname, wpos};
        obj.SetCoords(Coords::LOCAL);
        
        // Initial scale

        float scale {
          rand_toolkit::get_rand(scale-(scale/2.0f), scale+(scale/2.0f))};
        object::Scale(obj, {scale_, scale_, scale_});

        // Initial rotate

        const float k_max_roll = 15.0f;

        float roll {rand_toolkit::get_rand(0.0f, k_max_roll)};
        object::Rotate(obj, {0.0f, roll, 0.0f}, trig_);
        
        objects_.push_back(obj);
      }
    }
  }
}

}  // namespace anshub
  