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
#include "misc.h"


extern int global_screen_width;
extern int global_screen_height;

player_object * make_player_obj(float x,float y,float sx, float sy, float off_x, float off_y,bool movable, bool ghost, SpriteSheetDrawable tex_draw ){
  player_object * out = new player_object();
  out->x = x;
  out->y = y;
  out->set_aabb_data(sx,sy,off_x,off_y,movable,ghost);
  out->tex_draw = tex_draw;
  return out;
} 

physical_game_object * make_pgo(float x,float y,float sx, float sy, float off_x, float off_y,bool movable, bool ghost, SpriteSheetDrawable tex_draw ){
  physical_game_object * out = new physical_game_object();
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
  
  Texture treetex = make_texture("cow.png");
  Texture guy_tex  = make_texture("DormusSheet.png");
  BufferObject bobj = unit_rectangle_verts;
  BufferObject bobj2 = unit_rectangle_uvs;
  BufferObject bobj3 = unit_rectangle_inverse_uvs;
 
 SpriteSheetDrawable treeTD= SpriteSheetDrawable(treetex); 
  treeTD.load_animation_frame("test",48,30,0,0,0.2);
  treeTD.load_animation_frame("test",48,30,48,0,0.2);
  treeTD.set_animation("test");

  for(int i = 0; i < 15;i++){
    object_handler.load_object(make_pgo( (rand() % 50) + 50,  (rand() % 50) + 50, 20,5,0,5,true,false,treeTD));
  }  

  player_object dormus(14,10,10,10,0,7,true,false,guy_tex);
  player_object * a=&dormus; //= make_player_obj(10,5,5,5,0,5,true,false,treeTD);
  //a->tex_draw = guyss;
  key_ev k1;
  //mouse_move_spawner.register_listener(a); 
  //mouse_click_handler.register_listener(a);
  key_event_handler.register_listener(a);
  object_handler.load_object(a);
  

  int i = 0;
  int channel = 0;
  double mean[10];

  double start_t = get_time();
  while(true){
    double next_t = get_time();
    double dt = next_t - start_t;
    double dt_sleep = 1.0/60.0 - dt;
    start_t = next_t;
    if(dt_sleep > 0){
      sleep_sec(dt_sleep);
      start_t += dt_sleep;
    }
    i++;
    bind_shader(texture_shader);
    if(i%100 == 0){
      channel++;
      //play_audio_sample(&boom,channel%8);
    }
    //std::cout << (int)a->x << " " << (int)a->y << "\n";
      
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
