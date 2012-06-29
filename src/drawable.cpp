
#include "drawable.h"
#include "game_super.h"
#include <math.h>
#include <iostream>

bool z_compare(SpriteSheetDrawable * d1, SpriteSheetDrawable  * d2){
  return d1->z > d2->z;
}

frame::frame(int scalex,int scaley,int offx,int offy,double n_duration){
	scale[0]=scalex;
	scale[1]=scaley;
	offset[0]=offx;
	offset[1]=offy;
	duration=n_duration;
}

SpriteSheetDrawable::SpriteSheetDrawable(Texture tex)
{
  x = 0;
  y = 0;
  z = 0;
  this->tex = tex;
  gettimeofday(&start_time,NULL);
	
  current_frame=0;
	
  current_animation=animations.end();

}

void SpriteSheetDrawable::update(){
	if(current_animation!=animations.end()){
		timeval current_time;
		gettimeofday(&current_time,NULL);
		if((current_time.tv_sec - start_time.tv_sec)+(current_time.tv_usec - start_time.tv_usec) / 1000000.0f>current_animation->second[current_frame].duration){
			current_frame=(current_frame+1)%current_animation->second.size();
			gettimeofday(&start_time,NULL);
	
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

void SpriteSheetDrawable::set_animation(std::string new_animation){	
	current_animation=animations.find(new_animation);
	current_frame=0;
	gettimeofday(&start_time,NULL);
	if(current_animation==animations.end()){
		std::cout<<"Warning Animation not found!";
	}
}

DrawRequest SpriteSheetDrawable::MakeDrawRequest(){
  update();
  frame current_frame_i = current_animation->second[current_frame];
  
  DrawRequest out;
  out.x = x;
  out.y = y;
  out.uv_off_x = (float)current_frame_i.offset[0]/(float)tex.width;
  out.uv_off_y = (float)current_frame_i.offset[1]/(float)tex.height;
  out.uv_scale_x = (float)current_frame_i.scale[0]/(float)tex.width;
  out.uv_scale_y = (float)current_frame_i.scale[1]/(float)tex.height;
  out.vert_scale_x = current_frame_i.scale[0]/2;
  out.vert_scale_y = current_frame_i.scale[1]/2;
  out.tex = tex;
  return out;
  
}
	
