#include "drawable.h"
#include "gl_inc.h"
#include "game_super.h"
#include <math.h>

Drawable _default;
TextureDrawable::TextureDrawable(){
}
TextureDrawable::TextureDrawable(BufferObject verts, BufferObject uvs, Texture tex){
  this->verts = verts;
  this->uvs = uvs;
  this->tex = tex;
  x = 0; y = 0; z = 0;
}

void TextureDrawable::draw(GLProgram ptest){
  bind_buffer_object(verts,0);
  bind_buffer_object(uvs,1);
  bind_texture(tex,0);
  glUniform2f(glGetUniformLocation(ptest.gl_ref,"scale"),(float)1.0/global_screen_width,1.0/global_screen_height);
  glUniform2f(glGetUniformLocation(ptest.gl_ref,"object_scale"),tex.width,tex.height);
  glUniform2f(glGetUniformLocation(ptest.gl_ref,"off"),floor(x),floor(y));
  glDrawArrays(GL_TRIANGLE_FAN,0,4);
    
}


bool z_compare(Drawable * d1, Drawable * d2){
  return d1->z > d2->z;
}
