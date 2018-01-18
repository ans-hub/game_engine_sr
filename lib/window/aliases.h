// *************************************************************
// File:    aliases.cc
// Descr:   aliases for iogame lib entities
// Author:  Novoselov Anton @ 2017
// URL:     https://github.com/ans-hub/iogame_lib
// *************************************************************

#include <vector>
#include <memory>
#include <utility>

#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glxext.h>

#include <X11/Xutil.h>
#include <X11/extensions/Xrandr.h>

#ifndef IO_ALIASES_H
#define IO_ALIASES_H

namespace anshub {

typedef GLXContext (*glXCreateContextAttribsARBProc)(
  Display*, 
  GLXFBConfig, 
  GLXContext, 
  Bool, 
  const int*
);

// Custom deleters for some unique ptrs

auto xrr_deleter = [](XRRScreenConfiguration* x) { XRRFreeScreenConfigInfo(x); };
template<class X> auto x_deleter = [](X* x) { XFree(x); };

// Unique ptrs aliases

using XRRScreenPtr = 
  std::unique_ptr<XRRScreenConfiguration, decltype(xrr_deleter)>;
using VisualPtr = 
  std::unique_ptr<XVisualInfo, decltype(x_deleter<XVisualInfo>)>;

// Other some useful aliases

using Pair = std::pair<int,int>;
using CStr = const char*; 
using Visual = XVisualInfo;
using WAttribs = XWindowAttributes;
using SWAttribs = XSetWindowAttributes;
using FBAttrs = std::vector<int>;
using VIAttrs = std::vector<int>;
using CTAttrs = std::vector<int>;
using VModes = std::vector<std::pair<int,int>>;
using FBConfigs = std::vector<GLXFBConfig*>; // fb configs

} // namespace anshub

#endif  // IO_ALIASES_H