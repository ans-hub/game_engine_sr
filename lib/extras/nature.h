// *************************************************************
// File:    nature.h
// Descr:   represents simple nature on the terrain
// Author:  Novoselov Anton @ 2017
// *************************************************************

#ifndef GL_EXTRAS_NATURE_H
#define GL_EXTRAS_NATURE_H

#include <map>
#include <string>

#include "terrain.h"

#include "lib/render/exceptions.h"
#include "lib/render/gl_aliases.h"
#include "lib/render/gl_object.h" 
#include "lib/render/fx_colors.h"
#include "lib/render/gl_vertex.h"

#include "lib/data/bmp_loader.h"
#include "lib/math/trig.h"
#include "lib/system/rand_toolkit.h"

namespace anshub {

//****************************************************************************
// Default nature types
//****************************************************************************

enum class NatureTypes
{
  TREE_T1 = (255 << 8) + 1,       // Red component
  TREE_T2 = (220 << 8) + 1,
  ROCK_T1 = (255 << 16) + 1,      // Green component
  ROCK_T2 = (220 << 16) + 1,
  GRASS_T1 = (255 << 24) + 1,     // Blue component
  GRASS_T2 = (220 << 24) + 1

}; // enum class NatureTypes

//****************************************************************************
// Struct to recognize objects on the maps and create them
//****************************************************************************

struct Nature
{
  using MapString = std::map<uint, std::string>;
  using cTerrain = const Terrain;
  template<class T> using ObjsList = std::map<T, std::string>;

  Nature(const std::string& map_fname, float scale, const Terrain&, TrigTable&);

  template<class Enum>
  void  SetObjects(const std::map<Enum, std::string>&);
  
  void  RecognizeObjects();
  auto& GetObjects() { return objects_; }

private:
  Bitmap      map_;
  cTerrain&   terrain_;  
  V_GlObject  objects_;
  MapString   fnames_;
  float       scale_;
  TrigTable&  trig_;

}; // struct Nature

//****************************************************************************
// Inline and templates implementation
//****************************************************************************

// Set objects filenames for given type of object

template<class Enum>
inline void Nature::SetObjects(const std::map<Enum,std::string>& list)
{
  for (auto& item : list)
    fnames_[static_cast<uint>(item.first)] = item.second;
}

}  // namespace anshub

#endif // GL_EXTRAS_NATURE_H

// Note : Terrain class should accept Water object and Nature object
// as a part of himself (i.e., terrain.SetWater(water)) 