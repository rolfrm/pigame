#include "game_super.h"
#include "audio.h"
#include "gfx_basics.h"
#include <iostream>
int global_screen_width;
int global_screen_height;

extern unsigned int global_width;
extern unsigned int global_height;
float camera_x, camera_y, scale_x, scale_y;

void set_camera_position(int x, int y){
  camera_x = x;
  camera_y = y;
  std::cout << camera_x << "c" << camera_y << "\n";
}
void set_world_to_view(float scale_x, float scale_y, int view_pixels_width, int view_pixels_height){
 
}

void setup_shader_uniforms(GLProgram s){
  s.uniformf("scale",(float)1.0/global_screen_width, 1.0/global_screen_height);
  s.uniformf("camera",camera_x,camera_y);
}

GLProgram texture_shader;
void init_game(int window_width, int window_height, int width_pixels, int height_pixels){
  global_screen_width = width_pixels;
  global_screen_height = height_pixels;
  init_audio(44100, 16, 2);
  init_ogl(window_width,window_height);
  set_clearcolor(0.0f, 0.0f, 0.0f, 0.1f);

  texture_shader=make_program("test.vert","test.frag");
  texture_shader.bind_attr(0,"Pos");
  texture_shader.bind_attr(1,"UV_coord");
  texture_shader.link();
  bind_shader(texture_shader);  
  texture_shader.uniformf("scale",(float)1.0/global_screen_width, 1.0/global_screen_height);
}

mouse_position screen_pos_to_world_pos(mouse_position mpos){
  
  mpos.x = global_width/2 - mpos.x;
  mpos.y = mpos.y - global_height/2;
  mpos.x = mpos.x * (int)global_screen_width*2;
  mpos.y = mpos.y * (int)global_screen_height*2;
  mpos.x /= (int)global_width;
  mpos.y /= (int)global_height;
  mpos.x += camera_x;
  mpos.y += camera_y;
  return mpos;
}
