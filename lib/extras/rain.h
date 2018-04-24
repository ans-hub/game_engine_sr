// *************************************************************
// File:    rain.h
// Descr:   represents rain
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef GL_EXTRAS_RAIN_H
#define GL_EXTRAS_RAIN_H

#include <vector>

#include "lib/draw/gl_object.h"
#include "lib/draw/gl_vertex.h"
#include "lib/system/rand_toolkit.h"
#include "lib/math/vector.h"

namespace anshub {

//*****************************************************************************
// This is the blob - the part of rain
//*****************************************************************************

struct Blob : public GlObject
{
  friend struct Rain;

  Blob(float w);

private:
  Vector  local_pos_;
  bool    emplaced_;

}; // struct Blob

//*****************************************************************************
// And this is the rain - container of the Blobs =)
//*****************************************************************************

struct Rain : public GlObject
{
  using V_Blob = std::vector<Blob>;

  explicit Rain(int count, float w);
  void Process(cVector& obj_pos);
  auto& GetObjects() { return blobs_; }

private:
  int     count_;
  float   width_;
  V_Blob  blobs_;
  Vector  vel_;

}; // struct Rain

} // namespace anshub

#endif // GL_EXTRAS_RAIN_H