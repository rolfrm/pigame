#include "drawable.h"
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
  ptest.uniformf("scale",(float)1.0/global_screen_width,1.0/global_screen_height);
  ptest.uniformf("object_scale",tex.width,tex.height);
  ptest.uniformf("off",floor(x),floor(y));
  draw_buffers_triangle_fan(4);
    
}


bool z_compare(Drawable * d1, Drawable * d2){
  return d1->z > d2->z;
}
