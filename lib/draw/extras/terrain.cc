// *************************************************************
// File:    terrain.cc
// Descr:   generates terrain 
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#include "terrain.h"

namespace anshub {

// Creates GlObject that imitates terrain

Terrain::Terrain(const char* map_fname, const char* tex_fname, int div)
  : GlObject()
{
  // Load heights bitmaps and create object

  Bitmap map {map_fname};

  // Add texture to the object

  this->texture_ = std::make_shared<Bitmap>(tex_fname);
  this->textured_ = true;
  this->shading_ = Shading::FLAT;
  float dx_u {this->texture_->width() / map.width()}; 
  float dx_v {this->texture_->height() / map.height()}; 

  // Fill vertices of object. We suppose that map and tex sizes are the same,
  // otherwise throw Exception

  this->vxs_local_.reserve(map.height() * map.width());

  int half_h = map.height() / 2;
  int half_w = map.width() / 2;

  for (int i = 0; i < map.height(); ++i) {
    for (int k = 0; k < map.width(); ++k) {
      Vertex v {};
      float h = map.red_channel(k,i) / div;
      v.pos_ = Vector {(k - half_w), (int)h, -(i - half_h)};
      v.texture_ = Vector {k*dx_u, i*dx_v, 0.0f};
      this->vxs_local_.emplace_back(v);
    }
  }

  // Fill faces of the object

  int h = map.height();
  int w = map.width();

  for (int y = 0; y < map.height()-1; ++y) {
    for (int x = 0; x < map.width()-1; ++x) {
      
      Face f1 {this->vxs_local_, y*w+x, y*w+x+1, (y+1)*w+x};
      f1.color_ = FColor{color::White};
      this->faces_.push_back(f1);
      
      Face f2 {this->vxs_local_, y*w+x+1, (y+1)*w+x+1, (y+1)*w+x};
      f2.color_ = FColor{color::White};
      this->faces_.push_back(f2);      
    }
  }
}

} // namespace anshub