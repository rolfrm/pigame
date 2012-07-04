
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
#include <fstream>
#include <string>

int get_str_type(std::string str){
  int number_found = 0;
  for(int i=0; i < str.size();i++){
    if(isalpha(str[i])){
      return 2;
    }
    if(isdigit(str[i])){
      number_found = 1;
    }
  }
  return number_found; //0 or 1
}

std::string trim(const std::string& str) {
  size_t start = str.find_first_not_of(" \t\n\r");
  if(start == std::string::npos) return "";
 return str.substr(start, str.find_last_not_of(" \t\n\r") - start + 1);
}

float take_float(std::string *str){
  size_t start = str->find_first_of(" \t\n\r");
  float f =std::atof(str->c_str()); 
  *str = str->erase(0,start+1);
  return f;
}
frame string_to_frame(std::string str){
  
  float sx = take_float(&str);
  float sy = take_float(&str);
  float x = take_float(&str);
  float y = take_float(&str);
  float dt = take_float(&str);
  return frame(sx,sy,x,y,dt);
}

SpriteSheetDrawable SpriteSheetDrawable::from_ilist(std::string tex_path, std::vector<animation_line> alines){
  SpriteSheetDrawable ssd(get_texture(tex_path));
  for(int i = 0; i < alines.size();i++){
    ssd.load_animation_frames(alines[i].name,alines[i].frames);
  }
  return ssd;

}
	
SpriteSheetDrawable SpriteSheetDrawable::from_file(std::string path){
  std::ifstream file;
  file.open(path);
  if( !file.good() ){
    std::cout << "SpriteSheetDrawable::from_file: File not found\n";
    return SpriteSheetDrawable();
  }
  
  std::string texture_file;
  getline(file,texture_file);
  SpriteSheetDrawable ssd(get_texture(texture_file));
  std::vector<frame> animation_line;
  std::string animation_name = "";
  std::cout << "..\n";
  while(file.good()){
    std::string line;
    getline(file,line);
    int str_type = get_str_type(line);
    if(str_type == 0){
      continue;
    }
    line = trim(line);
    if(str_type == 1){
      frame f = string_to_frame(line);
      animation_line.push_back(f);
    }
    if(str_type == 2){
      if(animation_name != ""){
	ssd.load_animation_frames(animation_name,animation_line);
	animation_line.clear();
	std::cout << "Loading " << animation_name << "\n";
      }
      animation_name = line;
    }
    

  }
  if(animation_name != ""){
    ssd.load_animation_frames(animation_name,animation_line);
    ssd.set_animation(animation_name);
  }
  file.close();

  return ssd;
  
}
