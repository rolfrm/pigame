#include "EGL/egl.h"
typedef struct OpenGLContextInfo
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
