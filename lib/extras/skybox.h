// *************************************************************
// File:    skybox.h
// Descr:   represents skybox
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef GL_EXTRAS_SKYBOX_H
#define GL_EXTRAS_SKYBOX_H

#include "lib/draw/gl_object.h"
#include "lib/math/vector.h"

namespace anshub {

// Class represents skybox. We may load any textured object since
// this struct inside inverts the order of face points to make inverted
// normals

struct Skybox : public GlObject
{
  Skybox(const char* fname, cVector& pos);

}; // struct Skybox

}  // namespace anshub

#endif // GL_EXTRAS_SKYBOX_H