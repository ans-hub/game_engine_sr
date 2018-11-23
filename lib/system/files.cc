// *************************************************************
// File:    files.cc
// Descr:   files helpers
// Author:  Novoselov Anton @ 2017
// *************************************************************

#include "files.h"

namespace anshub {

bool file::IsExists(const std::string& fname)
{
  std::ifstream fs {fname};
  return fs.good();
}

} // namespace anshub