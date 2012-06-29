#pragma once
#include "buffer_objects.h"
#include "texture.h"
#include "GLProgram.h"
#include <map>
#include <vector>
#include <string>
#include <sys/time.h>

struct DrawRequest{
  float x,y;
  float uv_off_x,uv_off_y;
  float uv_scale_x, uv_scale_y;
  float vert_scale_x, vert_scale_y;
  Texture tex;
};



class Drawable{
public:
  float x,y,z;
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

  virtual void draw(GLProgram ptest);
};


class frame{
public:
	frame(int scalex,int scaley,int offx,int offy,double n_duration);
	
	void operator = (frame in){
		scale[0]=in.scale[0];
		scale[1]=in.scale[1];
		offset[0]=in.offset[0];
		offset[1]=in.offset[1];
		duration=in.duration;
	}
	
	int scale[2],offset[2];
	double duration;
};


class SpriteSheetDrawable: public TextureDrawable{
public:
	SpriteSheetDrawable(BufferObject verts, BufferObject uvs, Texture tex);
	SpriteSheetDrawable(){}
	void update();
	void draw(GLProgram ptest);
	DrawRequest MakeDrawRequest();
	void set_animation(std::string new_animation);
	void load_animation_frame(std::string name,int scalex,int scaley,int offx,int offy,double duration);
	
	int current_frame;
	timeval start_time;
	std::map< std::string, std::vector<frame> >::iterator current_animation;
	std::map< std::string, std::vector<frame> > animations;

};
	
bool z_compare(Drawable * d1, Drawable * d2);
