// *************************************************************
// File:    strings.h
// Descr:   string helpers
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef SYS_STRINGS_H
#define SYS_STRINGS_H

#include <string>
#include <algorithm>

namespace anshub {

namespace str {
  
  int Replace(std::string& str, const std::string& sub, const std::string& s);

} // namespace str

} // namespace anshub

#endif  // SYS_STRINGS_H