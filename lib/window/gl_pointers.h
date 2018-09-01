// *************************************************************
// File:    gl_pointers.h
// Descr:   opengl function pointers
// Author:  Novoselov Anton @ 2019
// URL:     https://github.com/ans-hub/iogame_lib
// *************************************************************

// Include this and call Gather... or use GL_GLEXT_PROTOTYPES
// under the linux instead

#ifndef IO_GL_POINTERS_H
#define IO_GL_POINTERS_H

#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glxext.h>
#include <GL/glu.h>

extern PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
extern PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
extern PFNGLGENBUFFERSPROC glGenBuffers;
extern PFNGLBINDBUFFERPROC glBindBuffer;
extern PFNGLBUFFERDATAPROC glBufferData;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
extern PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
extern PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
extern PFNGLCREATESHADERPROC glCreateShader;
extern PFNGLSHADERSOURCEPROC glShaderSource;
extern PFNGLCOMPILESHADERPROC glCompileShader;
extern PFNGLCREATEPROGRAMPROC glCreateProgram;
extern PFNGLATTACHSHADERPROC glAttachShader;
extern PFNGLLINKPROGRAMPROC glLinkProgram;
extern PFNGLUSEPROGRAMPROC glUseProgram;

namespace anshub {

namespace gl_pointers {

  bool GatherOpenglFunctions_v43();

} // namespace gl_pointers

} // anshub

#endif  // IO_GL_POINTERS_H