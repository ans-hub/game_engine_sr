// *************************************************************
// File:    helpers.h
// Descr:   low-level helpers to work with system directly
// Author:  Novoselov Anton @ 2017
// *************************************************************


#ifndef IO_HELPERS_H
#define IO_HELPERS_H

#ifndef GLX_GLXEXT_PROTOTYPES
#define GLX_GLXEXT_PROTOTYPES
#endif

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>   
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glxext.h>
#include <GL/glu.h>

#include <iostream>
#include <utility>
#include <vector>
#include <algorithm>
#include <limits>

#include "enums.h"
#include "aliases.h"
#include "exceptions.h"


namespace anshub {

namespace io_helpers {      // low-level io helpers

  // Defaults

  FBAttrs     GetDefaultFBAttribs();
  SWAttribs   GetDefaultWinAttribs(Display*, Window, XVisualInfo*);
  VIAttrs     GetDefaultVisualAttributes();
  CTAttrs     GetDefaultGlContextAttributes();

  // Config routines

  FBConfigs   GetFBConfigs(Display*, FBAttrs&);
  int         GetBestMSAAVisual(Display*, FBConfigs&);
  VisualPtr   ChooseGlVisual(Display*, GLXFBConfig&);
  VisualPtr   ChooseGlVisual(Display*);
  bool        FindFlagInFBAttrs(const FBAttrs&, int, int);

  // Context routines

  GLXContext  CreateGlContext(Display*, GLXFBConfig&, CTAttrs& attrs);
  GLXContext  CreateGlContext(Display*, Visual*);
  GLXPbuffer  CreatePBuffer(Display*, int, int, GLXFBConfig&);
  bool        ToggleVerticalSync(Display*, Window, bool);

  // Windows routines

  Window      CreateSmpWindow(Display*, Window, int, int, int, int, int);
  Window      CreateGlWindow(
              Display*, Window, Visual*, SWAttribs&, int, int, int, int);
    
  // Video modes and dimensions routines

  VModes      GetVideoModes(Display*, Window);
  int         GetCurrentVideoMode(Display*, Window);
  int         ChangeVideoMode(Display*, Window, int);
  void        GetWindowDimension(Display*, Window, int*, int*);
  int         FindNearestVideoMode(int, int);
  int         FindVideoMode(int, int);

  // Other usefull stuff

  void        ChangeWindowName(Display*, Window, const char*);
  Window      GetFocusedWindow(Display*);
  void        HideCursor(Display*, Window);
  void        UnhideCursor(Display*, Window);
  Pos         GetXYToMiddle(int, int);
  
  // Gl extensions routines

  const char* GetAllGlExtensions();
  const char* GetAllGlxExtensions(Display*);
  bool        IsExtensionSupported(const char*);
  bool        IsExtensionSupported(const char*, const char*);
  
  // Debug stuff 

  Pair        GetGlContextVersion();
  Pair        GetGlContextProfile();
  Pair        GetGlxVersion(Display*);
  void        PrintGlInfo(std::ostream&);

  // Sending notifies to WM

  bool  SendToggleFullscreenNotify(Display*, Window, Window);
  void  SendCloseWindowNotify(Display*, Window, Window);
  void  SendToggleOnTopNotify(Display*, Window, Window);
 
  // Wrappers around functions that returns pure pointers that needs to be freed
  
  namespace ptr {

    XRRScreenPtr XRRGetScreenInfo(Display*, Window);
    VisualPtr    glXGetVisualFromFBConfig(Display*, GLXFBConfig&);
    VisualPtr    glXChooseVisual(Display*, int, int*);
    
  } // namespace ptr

} // namespace io_helpers

std::ostream& operator<<(std::ostream&, std::pair<int,int>&);

} // namespace anshub

#endif  // IO_HELPERS_H

// Note: functions descriptions see in .cc file