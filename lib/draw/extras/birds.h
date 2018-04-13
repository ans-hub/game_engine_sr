// *************************************************************
// File:    birds.h
// Descr:   represents very simple birds
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef GL_EXTRAS_BIRDS_H
#define GL_EXTRAS_BIRDS_H

#include "../../system/rand_toolkit.h"
#include "../exceptions.h"
#include "../gl_aliases.h"
#include "../gl_object.h" 
#include "../fx_colors.h"
#include "../gl_vertex.h"

namespace anshub {

// Birds entity

struct Bird : public GlObject
{
  Bird(GlObject&& model, int init_delay, int world_width, int fly_height);
  void Process();

private: 

  void ResetPos();
  void ResetAim();
  void ResetVel();

  Vector  aim_;
  Vector  vel_;
  int     init_delay_;
  int     curr_delay_;
  int     world_width_;
  float   fly_height_;

}; // struct Bird

}  // namespace anshub

#endif // GL_EXTRAS_BIRDS_H