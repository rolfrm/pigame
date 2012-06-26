#pragma once
#include "buffer_objects.h"
#include "texture.h"
#include "GLProgram.h"

class Drawable{
public:
  int x,y,z;
  virtual void draw(GLProgram ptest){
    
  }
};

extern Drawable _default;
/*Stuff that wants to draw implements this.*/
class Drawer{
public:
  /*Return pointer to what you want to draw*/
  virtual Drawable * draw(){
    return &_default;
  }
};

class TextureDrawable: public Drawable{
public:
  TextureDrawable();
  TextureDrawable(BufferObject verts, BufferObject uvs, Texture tex);

  BufferObject verts, uvs;
  Texture tex;
  int uv_offset_x, uv_offeset_y;

  void draw(GLProgram ptest);
};

	
bool z_compare(Drawable * d1, Drawable * d2);
