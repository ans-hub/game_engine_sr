// *************************************************************
// File:    level.h
// Descr:   level class for mountain race game
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef LEVEL_H
#define LEVEL_H

#include <string>

#include "lib/audio/audio_fx.h"

#include "lib/data/cfg_loader.h"

#include "lib/draw/gl_bvh.h"
#include "lib/draw/fx_colors.h"
#include "lib/draw/gl_lights.h"
#include "lib/draw/gl_render_ctx.h"
#include "lib/draw/cameras/gl_camera.h"
#include "lib/draw/cameras/gl_camera_fol.h"

#include "lib/extras/cameraman.h"
#include "lib/extras/player.h"
#include "lib/extras/skybox.h"
#include "lib/extras/terrain.h"
#include "lib/extras/water.h"
#include "lib/extras/nature.h"
#include "lib/extras/rain.h"

#include "lib/math/trig.h"

namespace anshub {

// Level is just container of game objects

struct Level
{
  Level(const Config&);

  TrigTable trig_;

  // Audio objects

  AudioFx audio_;
  std::string engine_snd_;
  std::string crash_snd_;
  std::string ambient_snd_;
  AudioFx::Modifier engine_mod_;

  // Gameplay objects 

  CameraMan camman_;
  Player player_;
  CameraFol& follow_cam_;
  Skybox skybox_;
  Terrain terrain_;
  Water water_;
  Nature trees_;
  Nature nature_;
  Rain rain_;

  // Renderer objects

  Lights lights_all_;
  Lights lights_sky_;
  RenderContext render_ctx_;
  Bvh bvh_tree_;

private:

  void InitAudio(const Config&);
  void InitCamera(const Config&);
  void InitPlayer(const Config&);
  void InitFollowing(const Config&);
  void SetupSkybox(const Config&);
  void SetupTerrain(const Config&);
  void SetupTrees(const Config&);
  void SetupNature(const Config&);
  void SetupLights(const Config&);
  void SetupRenderContext(const Config&);

}; // struct Level 

} // namespace anshub

#endif // LEVEL_H