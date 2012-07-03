
#include "drawable.h"
#include "game_super.h"
#include <math.h>
#include <iostream>
#include "misc.h"
frame::frame(int scalex,int scaley,int offx,int offy,double n_duration){
	scale[0]=scalex;
	scale[1]=scaley;
	offset[0]=offx;
	offset[1]=offy;
	duration=n_duration;
}

void frame::operator=(frame in){
		scale[0]=in.scale[0];
		scale[1]=in.scale[1];
		offset[0]=in.offset[0];
		offset[1]=in.offset[1];
		duration=in.duration;
	}


SpriteSheetDrawable::SpriteSheetDrawable(Texture tex)
{
  this->tex = tex;
  
	
  current_frame=0;
	
  current_animation=animations.end();
  start_time = get_time();
}

SpriteSheetDrawable::SpriteSheetDrawable(){

}

void SpriteSheetDrawable::update(){
  if(current_animation!=animations.end()){
    double time_now = get_time();
    if(time_now - start_time > current_animation->second[current_frame].duration){
      current_frame=(current_frame+1)%current_animation->second.size();
      start_time = time_now;	  
    }
  }
}

void SpriteSheetDrawable::load_animation_frame(std::string name,int scalex,int scaley,int offx,int offy,double duration){
	std::map< std::string, std::vector<frame> >::iterator anim=animations.find(name);
	if(anim==animations.end()){
		std::vector<frame> temp;
		animations.insert(std::pair<std::string,std::vector<frame> >(name,temp));
		anim=animations.find(name);
	}
	anim->second.push_back(frame(scalex,scaley,offx,offy,duration));
}

void SpriteSheetDrawable::load_animation_frames(std::string name,std::vector<frame> frames){
  animations.insert(std::pair<std::string, std::vector<frame> >(name,frames));
}


void SpriteSheetDrawable::set_animation(std::string new_animation){	
	current_animation=animations.find(new_animation);
	current_frame=0;
	start_time = 0.0;
	if(current_animation==animations.end()){
		std::cout<<"Warning Animation not found!";
	}
	current_animation_length = 0.0;
	int cal = current_animation->second.size();
	for(int i = 0; i < cal;i++){
	  current_animation_length += current_animation->second[i].duration;
	}
}

DrawRequest SpriteSheetDrawable::get_draw_request(frame current_frame_i){
  DrawRequest out;
  out.uv_off_x = (float)current_frame_i.offset[0]/(float)tex.width;
  out.uv_off_y = (float)current_frame_i.offset[1]/(float)tex.height;
  out.uv_scale_x = (float)current_frame_i.scale[0]/(float)tex.width;
  out.uv_scale_y = (float)current_frame_i.scale[1]/(float)tex.height;
  out.vert_scale_x = current_frame_i.scale[0]/2;
  out.vert_scale_y = current_frame_i.scale[1]/2;
  out.tex = tex;
  return out;
  
}

DrawRequest SpriteSheetDrawable::special_draw_request(float x, float y, float time){
  
  float rest_time = fmod(time,current_animation_length);
  int i = 0;
  for(; rest_time > current_animation->second[i].duration; i +=1, rest_time -= current_animation->second[i].duration){
  }
  
  DrawRequest out = get_draw_request(current_animation->second[i]);
  out.x = x;
  out.y = y;
  return out;
}
  


DrawRequest SpriteSheetDrawable::make_draw_request(float x, float y){
  update();
  frame current_frame_i = current_animation->second[current_frame];
  
  DrawRequest out = get_draw_request(current_frame_i);
  out.x = x;
  out.y = y;
  return out;
  
}
	
