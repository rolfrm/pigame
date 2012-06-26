#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <vector>
#include <list>
#include <math.h>
#include <iostream>
#include "CollisionDetection.h"

#include "texture.h"
#include "GLProgram.h"
#include "buffer_objects.h"
#include "frame_buffer.h"

#include "event_handling.h"
#include "input_events.h"
#include "game_objects.h"
#include "game.h"
#include "audio.h"
#include "gfx_basics.h"

extern int global_screen_width;
extern int global_screen_height;

float vertex[8] = {
	-1.0f,1.0f,
	1.0f,1.0f,
	1.0f,-1.0f,
	-1.0f,-1.0f,
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

player_object * make_player_obj(float x,float y,float sx, float sy, float off_x, float off_y,bool movable, bool ghost, TextureDrawable tex_draw ){
  player_object * out = new player_object();
  out->x = x;
  out->y = y;
  out->set_aabb_data(sx,sy,off_x,off_y,movable,ghost);
  out->tex_draw = tex_draw;
  return out;
} 


int main(){

  global_screen_width = 256;
  global_screen_height = 256;

  init_audio(44100, 16, 2);
  init_ogl(512,512);
  
  set_clearcolor(0.0f, 0.0f, 0.0f, 0.1f);

  GLProgram ptest=make_program("test.vert","test.frag");
  ptest.bind_attr(0,"Pos");
  ptest.bind_attr(1,"UV_coord");
  ptest.link();
  bind_shader(ptest);  
  ptest.uniformf("scale",(float)1.0/global_screen_width, 1.0/global_screen_height);
  
  ObjectHandler object_handler;
  object_handler.current_shader = ptest;

  Music m1("ko-ko.ogg");
  Music m2(m1);
  AudioSample boom("boom.ogg");
  play_music(m2);
  
  int asd[512*512];
  Texture fb_tex = make_texture((void *) asd,global_screen_width,global_screen_height,3);  
  FrameBuffer fb(fb_tex);
  
  Texture treetex = make_texture("tree123.png");
  BufferObject bobj = make_buffer_object((void *)vertex,4,2,FLOAT);
  BufferObject bobj2 = make_buffer_object((void *)uv,4,2,FLOAT);
  BufferObject bobj3 = make_buffer_object((void *)uv2,4,2,FLOAT);
  
  TextureDrawable treeTD= TextureDrawable(bobj,bobj2,treetex); 
  
  player_object * a = make_player_obj(10,20,30,15,0,5,true,false,treeTD);
  
  mouse_move_spawner.register_listener(a); 
  mouse_click_handler.register_listener(a);
  object_handler.load_object(a);
  
  for(int i = 0; i < 5;i++){
    object_handler.load_object(make_player_obj(- (rand()% 128)*4 + 128, - (rand()% 128)*2 + 200, 30,15,0,5,true,false,treeTD));
  }
  int i = 0;
  int channel = 0;
  while(true){
    i++;
    if(i%100 == 0){
      channel++;
      //play_audio_sample(&boom,channel%8);
    }
    bind_framebuffer(fb);
    clear_bound_framebuffer();

    //** Go gameloop!
    object_handler.gameloop();
    //
    unbind_framebuffer();
    bind_buffer_object(bobj3,1);
    bind_buffer_object(bobj,0);

    ptest.uniformf("object_scale",global_screen_width,global_screen_height);
    ptest.uniformf("off",0.0,0.0);
    bind_texture(fb.textures[0],0);

    draw_buffers_triangle_fan(4);
    
    swapbuffers();
  }
  
  return 0;
}
