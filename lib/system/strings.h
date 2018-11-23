// *************************************************************
// File:    strings.h
// Descr:   string helpers
// Author:  Novoselov Anton @ 2017
// *************************************************************

#ifndef SYS_STRINGS_H
#define SYS_STRINGS_H

#include <string>
#include <algorithm>

namespace anshub {

namespace str {

  using Str = std::string;

  int Replace(Str& str, const Str& sub, const Str& replace);
  Str Replace(const Str& str, const Str& sub, const Str& replace);

} // namespace str

} // namespace anshub

#endif  // SYS_STRINGS_H