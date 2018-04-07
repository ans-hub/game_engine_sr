// *************************************************************
// File:    nature.h
// Descr:   represents nature of the terrain
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef GL_EXTRAS_NATURE_H
#define GL_EXTRAS_NATURE_H

#include <map>
#include <string>

#include "terrain.h"

#include "../exceptions.h"
#include "../gl_aliases.h"
#include "../gl_object.h" 
#include "../fx_colors.h"
#include "../gl_vertex.h"

#include "../../data/bmp_loader.h"
#include "../../math/trig.h"
#include "../../system/rand_toolkit.h"

namespace anshub {

// Represents class that is simple container of GlObjects

struct Nature
{
  using MapString = std::map<uint, std::string>;
  using cTerrain = const Terrain;

  enum Types{
    TREE_T1 = (255 << 8) + 1,       // Red component
    TREE_T2 = (220 << 8) + 1,
    ROCK_T1 = (255 << 16) + 1,      // Green component
    ROCK_T2 = (220 << 16) + 1,
    GRASS_T1 = (255 << 24) + 1,     // Blue component
    GRASS_T2 = (220 << 24) + 1
  };

  explicit Nature(const std::string& map_fname, const Terrain&, TrigTable&);

  void SetObjects(Types, const std::string& obj_fname);
  void RecognizeObjects();
  auto& GetObjects() { return objects_; }

private:
  Bitmap      map_;
  cTerrain&   terrain_;  
  V_GlObject  objects_;
  MapString   fnames_;
  TrigTable&  trig_;

}; // struct Nature

}  // namespace anshub

#endif // GL_EXTRAS_NATURE_H

// Note : the best way is to make V_GlObject as separate object (not
// hust alias as now) + Terrain class should accet Water object and
// Nature object as part of himself (i.e., terrain.SetWater(water)) 