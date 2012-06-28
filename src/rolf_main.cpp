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
#include "game_super.h"
#include "audio.h"
#include "gfx_basics.h"


extern int global_screen_width;
extern int global_screen_height;

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

player_object * make_player_obj(float x,float y,float sx, float sy, float off_x, float off_y,bool movable, bool ghost, TextureDrawable tex_draw ){
  player_object * out = new player_object();
  out->x = x;
  out->y = y;
  out->set_aabb_data(sx,sy,off_x,off_y,movable,ghost);
  out->tex_draw = tex_draw;
  return out;
} 

class key_ev: public EventListener<KeyEvent>{
  void handle_event(KeyEvent kev){
    switch(kev.key){
    case 283:set_camera_position(camera_x,camera_y - 1);break; //Up
    case 285:set_camera_position(camera_x + 1,camera_y );break; //left
    case 284:set_camera_position(camera_x,camera_y + 1 );break; //down
    case 286:set_camera_position(camera_x - 1,camera_y );break; //right
    }
  }
};

int main(){
  init_game(512,512,128,128);
  set_clearcolor(0.1,0.5,0.0,1.0);
  GLProgram ptest = texture_shader;
  
  ObjectHandler object_handler;
  object_handler.current_shader = ptest;

  Music m1("ko-ko.ogg");
  Music m2(m1);
  AudioSample boom("boom.ogg");
  //play_music(m2);
  
  Texture fb_tex = make_texture((void *) NULL,global_screen_width,global_screen_height,3);  
  FrameBuffer fb(fb_tex);
  
  Texture treetex = make_texture("tree123.png");
  BufferObject bobj = make_buffer_object((void *)vertex,4,2,FLOAT);
  BufferObject bobj2 = make_buffer_object((void *)uv,4,2,FLOAT);
  BufferObject bobj3 = make_buffer_object((void *)uv2,4,2,FLOAT);
  
  TextureDrawable treeTD= TextureDrawable(bobj,bobj2,treetex); 
  
  player_object * a = make_player_obj(10,20,30,15,0,5,true,false,treeTD);
  key_ev k1;
  mouse_move_spawner.register_listener(a); 
  mouse_click_handler.register_listener(a);
  key_event_handler.register_listener(&k1);
  object_handler.load_object(a);
  
  for(int i = 0; i < 5;i++){
    object_handler.load_object(make_player_obj(- (rand()% 128)*4 + 128, - (rand()% 128)*2 + 200, 30,15,0,5,true,false,treeTD));
  }
  int i = 0;
  int channel = 0;
  while(true){
    i++;
    bind_shader(texture_shader);
    if(i%100 == 0){
      channel++;
      //play_audio_sample(&boom,channel%8);
    }
    std::cout << (int)a->x << " " << (int)a->y << "\n";
      
    bind_framebuffer(fb);
    clear_bound_framebuffer();

    //** Go gameloop!
    object_handler.gameloop();
    //
    
    unbind_framebuffer();
    bind_shader(screen_drawer);
    bind_buffer_object(bobj3,1);
    bind_buffer_object(bobj,0);
    screen_drawer.uniformf("camera",0,0);
    screen_drawer.uniformf("scale",1.0,1.0);
    screen_drawer.uniformf("object_scale",1.0,1.0);
    screen_drawer.uniformf("off",0,0);
    bind_texture(fb.textures[0],0);

    draw_buffers_triangle_fan(4);
    
    swapbuffers();
  }
  
  return 0;
}
