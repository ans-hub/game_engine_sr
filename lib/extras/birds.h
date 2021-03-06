// *************************************************************
// File:    birds.h
// Descr:   represents very simple birds
// Author:  Novoselov Anton @ 2017
// *************************************************************

#ifndef GL_EXTRAS_BIRDS_H
#define GL_EXTRAS_BIRDS_H

#include "lib/system/rand_toolkit.h"
#include "lib/render/exceptions.h"
#include "lib/render/gl_aliases.h"
#include "lib/render/gl_object.h" 
#include "lib/render/fx_colors.h"
#include "lib/render/gl_vertex.h"

namespace anshub {

//*************************************************************************
// Birds entity
//*************************************************************************

struct Bird : public GlObject
{
  Bird(GlObject&& model, int init_delay, int world_width, float fly_height);
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