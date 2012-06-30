#include "game_super.h"
#include "audio.h"
#include "gfx_basics.h"

#include <iostream>
int global_screen_width;
int global_screen_height;

unsigned int global_width, global_height;


float camera_x, camera_y, scale_x, scale_y;

void set_camera_position(int x, int y){
  camera_x = x;
  camera_y = y;
}

void move_camera(int dx, int dy){
  camera_x += dx;
  camera_y +=dy;
}

void set_world_to_view(float scale_x, float scale_y, int view_pixels_width, int view_pixels_height){
 
}

void setup_shader_uniforms(GLProgram s){
  s.uniformf("scale",(float)2.0/global_screen_width, 2.0/global_screen_height);
  s.uniformf("camera",camera_x,camera_y);
}

float vertex[8] = {
	-1.0f,1.0f,
	1.0f,1.0f,
	1.0f,-1.0f,
	-1.0f,-1.0f
};

float uv[8] = {
	1.0f,1.0f,
	0.0f,1.0f,
	0.0f,0.0f,
	1.0f,0.0f,
};

float uv2[8] = {
	1.0f,0.0f,
	0.0f,0.0f,
	0.0f,1.0f,
	1.0f,1.0f,
};

BufferObject unit_rectangle_verts;
BufferObject unit_rectangle_uvs;
BufferObject unit_rectangle_inverse_uvs;


GLProgram screen_drawer;
GLProgram texture_shader;
void init_game(int window_width, int window_height, int width_pixels, int height_pixels){
  global_screen_width = width_pixels;
  global_screen_height = height_pixels;

  global_width=window_width;
  global_height=window_height;
  
  init_audio(44100, 16, 2);
  init_ogl(window_width,window_height);
  set_clearcolor(0.0f, 0.0f, 0.0f, 0.1f);

  texture_shader=make_program("sprite.vert","sprite.frag");
  texture_shader.bind_attr(0,"Pos");
  texture_shader.bind_attr(1,"UV_coord");
  texture_shader.link();
  bind_shader(texture_shader);  
  texture_shader.uniformf("scale",(float)1.0/global_screen_width, 1.0/global_screen_height);

  screen_drawer = make_program("test.vert","test.frag");
  screen_drawer.bind_attr(0,"Pos");
  screen_drawer.bind_attr(1,"UV_coord");
  screen_drawer.link();
  screen_drawer.uniformf("scale", (float)1.0/global_screen_width, (float) 1.0/global_screen_height);
  
  unit_rectangle_verts= make_buffer_object((void *)vertex,4,2,FLOAT);
  unit_rectangle_uvs = make_buffer_object((void *)uv,4,2,FLOAT);
  unit_rectangle_inverse_uvs  = make_buffer_object((void *)uv2,4,2,FLOAT);
 
}

mouse_position screen_pos_to_world_pos(mouse_position mpos){
  
  mpos.x = global_width/2 - mpos.x;
  mpos.y = mpos.y - global_height/2;
  mpos.x = mpos.x * (int)global_screen_width;
  mpos.y = mpos.y * (int)global_screen_height;
  mpos.x /= (int)global_width;
  mpos.y /= (int)global_height;
  mpos.x += camera_x;
  mpos.y += camera_y;
  return mpos;
}

