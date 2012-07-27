#include "gl_inc.h"

void set_clearcolor(float r, float g, float b, float a){
  glClearColor(r,g,b,a);
}

void set_line_width(float size){
  glLineWidth(size);
}
