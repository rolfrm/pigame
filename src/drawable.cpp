#include "drawable.h"
#include "game_super.h"
#include <math.h>
#include <iostream>

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
  bind_shader(texture_shader);
  setup_shader_uniforms(texture_shader);
  bind_buffer_object(verts,0);
  bind_buffer_object(uvs,1);
  bind_texture(tex,0);
  ptest.uniformf("object_scale",tex.width,tex.height);
  ptest.uniformf("off",floor(x),floor(y));
  draw_buffers_triangle_fan(4);
    
}


bool z_compare(Drawable * d1, Drawable * d2){
  return d1->z > d2->z;
}

frame::frame(int scalex,int scaley,int offx,int offy,double n_duration){
	scale[0]=scalex;
	scale[1]=scaley;
	offset[0]=offx;
	offset[1]=offy;
	duration=n_duration;
}

SpriteSheetDrawable::SpriteSheetDrawable(BufferObject verts, BufferObject uvs, Texture tex):
TextureDrawable(verts,uvs,tex)
{
	gettimeofday(&start_time,NULL);
	
	current_frame=0;
	
	current_animation=animations.end();

}

void SpriteSheetDrawable::draw(GLProgram ptest){
  update();
  bind_shader(ptest);
  bind_buffer_object(verts,0);
  bind_buffer_object(uvs,1);
  bind_texture(tex,0);
  frame tempframe=current_animation->second[current_frame];
  ptest.uniformf("scale",(float)1.0/global_screen_width,1.0/global_screen_height); //temporary world2view fix
  ptest.uniformf("object_scale",tempframe.scale[0],tempframe.scale[1]);
  ptest.uniformf("off",x,y);
  ptest.uniformf("uv_scale",(float)tempframe.scale[0]/(float)tex.width,(float)tempframe.scale[1]/(float)tex.height);
  ptest.uniformf("uv_offset",(float)tempframe.offset[0]/(float)tex.width,(float)tempframe.offset[1]/(float)tex.height);
  draw_buffers_triangle_fan(4);
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
