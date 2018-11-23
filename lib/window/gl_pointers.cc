// *************************************************************
// File:    gl_pointers.cc
// Descr:   opengl function pointers
// Author:  Novoselov Anton @ 2017
// *************************************************************

#include "gl_pointers.h"

PFNGLGENVERTEXARRAYSPROC glGenVertexArrays = nullptr;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray = nullptr;
PFNGLGENBUFFERSPROC glGenBuffers = nullptr;
PFNGLBINDBUFFERPROC glBindBuffer = nullptr;
PFNGLBUFFERDATAPROC glBufferData = nullptr;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray = nullptr;
PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray = nullptr;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer = nullptr;
PFNGLCREATESHADERPROC glCreateShader = nullptr;
PFNGLSHADERSOURCEPROC glShaderSource = nullptr;
PFNGLCOMPILESHADERPROC glCompileShader = nullptr;
PFNGLCREATEPROGRAMPROC glCreateProgram = nullptr;
PFNGLATTACHSHADERPROC glAttachShader = nullptr;
PFNGLLINKPROGRAMPROC glLinkProgram = nullptr;
PFNGLUSEPROGRAMPROC glUseProgram = nullptr;

namespace anshub {

namespace gl_pointers {

  bool GatherOpenglFunctions_v43()
  {
    glGenVertexArrays =
      (PFNGLGENVERTEXARRAYSPROC)glXGetProcAddress((const GLubyte*)"glGenVertexArrays");
    glBindVertexArray =
      (PFNGLBINDVERTEXARRAYPROC)glXGetProcAddress((const GLubyte*)"glBindVertexArray");
    glGenBuffers =
      (PFNGLGENBUFFERSPROC)glXGetProcAddress((const GLubyte*)"glGenBuffers");
    glBindBuffer =
      (PFNGLBINDBUFFERPROC)glXGetProcAddress((const GLubyte*)"glBindBuffer");
    glBufferData =
      (PFNGLBUFFERDATAPROC)glXGetProcAddress((const GLubyte*)"glBufferData");
    glEnableVertexAttribArray = 
      (PFNGLENABLEVERTEXATTRIBARRAYPROC)glXGetProcAddress(
        (const GLubyte*)"glEnableVertexAttribArray");
    glDisableVertexAttribArray = 
      (PFNGLDISABLEVERTEXATTRIBARRAYPROC)glXGetProcAddress(
        (const GLubyte*)"glDisableVertexAttribArray");
    glVertexAttribPointer = 
      (PFNGLVERTEXATTRIBPOINTERPROC)glXGetProcAddress(
        (const GLubyte*)"glVertexAttribPointer");
    glCreateShader =
      (PFNGLCREATESHADERPROC)glXGetProcAddress((const GLubyte*)"glCreateShader");
    glShaderSource =
      (PFNGLSHADERSOURCEPROC)glXGetProcAddress((const GLubyte*)"glShaderSource");
    glCompileShader =
      (PFNGLCOMPILESHADERPROC)glXGetProcAddress((const GLubyte*)"glCompileShader");
    glCreateProgram =
      (PFNGLCREATEPROGRAMPROC)glXGetProcAddress((const GLubyte*)"glCreateProgram");
    glAttachShader =
      (PFNGLATTACHSHADERPROC)glXGetProcAddress((const GLubyte*)"glAttachShader");
    glLinkProgram =
      (PFNGLLINKPROGRAMPROC)glXGetProcAddress((const GLubyte*)"glLinkProgram");
    glUseProgram =
      (PFNGLUSEPROGRAMPROC)glXGetProcAddress((const GLubyte*)"glUseProgram");

    return true;
  }

} // namespace gl_pointers

} // namespace anshub
