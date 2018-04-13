// *************************************************************
// File:    config.h
// Descr:   config struct to load terrains
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef GC_CONF_TERR_H
#define GC_CONF_TERR_H

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <iomanip> 
#include <map>
#include <stdexcept>

#include "lib/math/vector.h"

namespace anshub {

// Represents config for terrain app

struct Config
{
  using V_Float = std::vector<float>;
  using Str = std::string;
  using Vector = anshub::Vector;

  Config(const char* cfg_fname);

  float   GetFloat(const Str& name) { return f_[name]; }
  float   GetBool(const Str& name) { return b_[name]; }
  Str     GetString(const Str& name) { return s_[name]; }
  Vector  GetVector3d(const Str& name) { return vv_[name]; }
  V_Float GetVectorF(const Str& name) { return vf_[name]; }

  std::map<Str, float>    f_;   // to store float values
  std::map<Str, bool>     b_;   // to store bool values
  std::map<Str, Str>      s_;   // to store string values
  std::map<Str, Vector>   vv_;  // to store Vector values
  std::map<Str, V_Float>  vf_;  // to store std::vector<float> values

}; // struct Config

struct CfgExcept : std::runtime_error
{
  CfgExcept(const char* msg) : std::runtime_error(msg) { }

}; // struct CfgExcept

// Constructor implementation

inline Config::Config(const char* cfg_fname)
{
  std::ifstream fs {cfg_fname};
  if (!fs)
    throw CfgExcept("Can`t load file");

  std::string   line {""};
  std::string   type {""};
  std::string   param {""};
  float         val_f {};

  while(std::getline(fs, line))
  {
    std::istringstream stream {line};
    stream >> type >> param;
    
    if (type == "f")
      stream >> f_[param];
    
    else if (type == "b")
      stream >> b_[param];

    else if (type == "s")
      stream >> s_[param];
      
    else if (type == "v")
    {
      Vector v {};
      while (stream >> v.x >> v.y >> v.z) { }
      vv_[param] = v;
    }
    else if (type == "a")
    {
      std::vector<float> v {};
      while (stream >> val_f)
        v.push_back(val_f);
      vf_[param] = v;
    }
  }   
}

} // namespace anshub

#endif // GC_CONF_TERR_H