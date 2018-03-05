// *************************************************************
// File:    strings.cc
// Descr:   string helpers
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#include "strings.h"

namespace anshub {

int str::Replace(std::string& str, const std::string& sub, const std::string& s)
{
  int cnt {0};
  while(true)
  {
    auto found = str.find(sub);
    if (found != std::string::npos) {
      str.replace(found, sub.length(), s);
      ++cnt;
    }
    else
      break;
  }
  return cnt;
}

}  // namespace anshub