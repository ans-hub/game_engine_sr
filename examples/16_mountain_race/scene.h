// *************************************************************
// File:    scene.h
// Descr:   scene class for mountain race game
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef SCENE_H
#define SCENE_H

#include "level.h"

#include "lib/data/cfg_loader.h"
#include "lib/draw/gl_draw.h"
#include "lib/draw/gl_render_ctx.h"
#include "lib/draw/cameras/gl_camera.h"
#include "lib/window/gl_window.h"

namespace anshub {

struct Scene
{
  Scene(const Config&, GlWindow&, Level&);
  void Build();

  auto GetObjectsCulled() const { return objects_culled_; }
  auto GetTrianglesCulled() const { return triangles_culled_; }
  auto GetSurfacesHidden() const { return hidden_surfaces_; }
  auto GetTrianglesCount() const { return tris_base_.size(); }

private:
  GlWindow& win_;
  Level& level_;

  V_Triangle tris_base_;
  V_Triangle tris_sky_;
  V_TrianglePtr tris_ptrs_;

  int hidden_surfaces_;
  int objects_culled_;
  int triangles_culled_;

  void BuildPlayer(const GlCamera&);
  void BuildSkybox(const GlCamera&);
  void BuildWater(const GlCamera&);
  void BuildNature(const GlCamera&);
  void BuildRain(const GlCamera&);
  void BuildTerrain(const GlCamera&);
  
  void MakeTriangles();
  void ProcessTriangles(const GlCamera&);

}; // struct Scene 

}  // namespace anshub

#endif // SCENE_H