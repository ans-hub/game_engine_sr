// *************************************************************
// File:    cfg_loader.h
// Descr:   struct to read config files
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef GC_CFG_LOADER_H
#define GC_CFG_LOADER_H

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <iomanip> 
#include <vector> 
#include <map>
#include <stdexcept>

#include "lib/math/vector.h"

namespace anshub {

//****************************************************************************
// Represents config for demo applications
//****************************************************************************

struct Config
{
  using V_Float = std::vector<float>;
  using Str = std::string;

  Config(const char* cfg_fname);
  template<class T>
    auto Get(const Str& name) const;

private:
  std::map<Str, int>      integers_;
  std::map<Str, float>    floats_;
  std::map<Str, bool>     bools_;
  std::map<Str, Str>      strings_;
  std::map<Str, Vector>   vectors3d_;
  std::map<Str, V_Float>  vectorsf_;

}; // struct Config

struct CfgExcept : std::runtime_error
{
  CfgExcept(const char* msg) : std::runtime_error(msg) { }

}; // struct CfgExcept

//****************************************************************************
// Inline member functions implementation
//****************************************************************************

inline Config::Config(const char* cfg_fname)
{
  std::ifstream fs {cfg_fname};
  if (!fs)
    throw CfgExcept("Can`t load file");

  std::string   line  {""};
  std::string   type  {""};
  std::string   param {""};
  float         val_f {};

  while(std::getline(fs, line))
  {
    std::istringstream stream {line};
    stream >> type >> param;
    
    if (type == "f")
      stream >> floats_[param];
    else if (type == "i")
      stream >> integers_[param];
    else if (type == "b")
      stream >> bools_[param];
    else if (type == "s")
      stream >> strings_[param];
    else if (type == "v")
    {
      Vector v {};
      while (stream >> v.x >> v.y >> v.z) { }
      vectors3d_[param] = v;
    }
    else if (type == "a")
    {
      std::vector<float> v {};
      while (stream >> val_f)
        v.push_back(val_f);
      vectorsf_[param] = v;
    }
  }   
}

// Specialized template member functions

template<>
inline auto Config::Get<int>(const Str& name) const
{ 
  return integers_.at(name);
}

template<>
inline auto Config::Get<float>(const Str& name) const
{
  return floats_.at(name);
}

template<>
inline auto Config::Get<bool>(const Str& name) const
{
  return bools_.at(name);
}

template<>
inline auto Config::Get<std::string>(const Str& name) const
{
  return strings_.at(name);
}

template<>
inline auto Config::Get<Vector>(const Str& name) const
{
  return vectors3d_.at(name);
}

template<>
inline auto Config::Get<Config::V_Float>(const Str& name) const
{
  return vectorsf_.at(name);
}

} // namespace anshub

#endif // GC_CFG_LOADER_H