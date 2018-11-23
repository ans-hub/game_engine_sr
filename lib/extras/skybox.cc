// *************************************************************
// File:    skybox.cc
// Descr:   represents skybox 
// Author:  Novoselov Anton @ 2017
// *************************************************************

#include "skybox.h"

namespace anshub {

// Creates GlObject that imitates skybox

Skybox::Skybox(const char* fname, cVector& pos)
  : GlObject{fname, pos}
{
  // Invert face vertices order

  for (auto& f : faces_)
    std::swap(f.vxs_[0], f.vxs_[2]);

  // Set shading for lighting purposes

  this->shading_ = Shading::FLAT;
}

} // namespace anshub