#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "EGL/egl.h"
#include "EGL/eglext.h"


typedef struct
{
   uint32_t screen_width;
   uint32_t screen_height;
// OpenGL|ES objects
   EGLDisplay display;
   EGLSurface surface;
   EGLContext context;
} OpenGLContextInfo;

void init_ogl();

void swapbuffers();
