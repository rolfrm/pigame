#pragma once
#include "buffer_objects.h"
#include "texture.h"
#include "GLProgram.h"
#include <map>
#include <vector>
#include <string>

struct DrawRequest{
  float x,y;
  float uv_off_x,uv_off_y;
  float uv_scale_x, uv_scale_y;
  float vert_scale_x, vert_scale_y;
  Texture tex;
};

class frame{
public:
	frame(int scalex,int scaley,int offx,int offy,double n_duration);
	
	void operator = (frame in);	
	int scale[2],offset[2];
	double duration;
};



class SpriteSheetDrawable{
public:
  Texture tex;

  SpriteSheetDrawable(Texture tex);
  SpriteSheetDrawable();
  void update();
  DrawRequest make_draw_request(float x = 0, float y = 0);
  DrawRequest special_draw_request(float x = 0, float y = 0, float time = 0);
  DrawRequest get_draw_request(frame current_frame);
  void set_animation(std::string new_animation);
  void load_animation_frame(std::string name,int scalex,int scaley,int offx,int offy,double duration);
  void load_animation_frames(std::string name,std::vector<frame> frames);
  int current_frame;
  double start_time;
  double current_animation_length;
  std::map< std::string, std::vector<frame> >::iterator current_animation;
  std::map< std::string, std::vector<frame> > animations;

};

/*Stuff that wants to draw implements this.*/
class Drawer{
public:
  /*Return pointer to what you want to draw*/
  virtual DrawRequest draw() = 0;
};
