// *************************************************************
// File:    strings.cc
// Descr:   string helpers
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#include "strings.h"

namespace anshub {

// Replaces all "sub" in "str" with "replace" and returns count of substitutes

int str::Replace(
  std::string& str, const std::string& sub, const std::string& replace)
{
  int cnt {0};
  while(true)
  {
    auto found = str.find(sub);
    if (found != std::string::npos) {
      str.replace(found, sub.length(), replace);
      ++cnt;
    }
    else
      break;
  }
  return cnt;
}

// The same as above but return new string

std::string str::Replace(
  const std::string& str, const std::string& sub, const std::string& replace)
{
  std::string res {str};
  while(true)
  {
    auto found = res.find(sub);
    if (found != std::string::npos)
      res.replace(found, sub.length(), replace);
    else
      break;
  }
  return res;
}

}  // namespace anshub