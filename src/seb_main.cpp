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

class Cow: public physical_game_object{
public:
  static SpriteSheetDrawable ssd;
  static int inited;
  Cow(){
    if(inited == -1){
      ssd = SpriteSheetDrawable(make_texture("cow.png"));
      ssd.load_animation_frame("test",48,30,0,0,0.2);
      ssd.load_animation_frame("test",48,30,48,0,0.2);
      ssd.set_animation("test");
      inited = 1;
    }
    tex_draw = ssd;
    set_aabb_data(20,10,0,5,true,false);
  }
};
SpriteSheetDrawable Cow::ssd;
int Cow::inited = -1;




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

#define DOTILE(x,y,z) Tile(z,tsf.make_animated_tile(x,y))
class TileCreator{
public:
  Tile tileset[100];
  TileCreator(){
    TileSpriteFactory tsf("grass_tiles.png", 18,10);

    tileset[0] = DOTILE(0,{0.2},true);
    tileset[1] = DOTILE(1,{0.2},true);
    tileset[2] = DOTILE(2,{0.2},false);
    tileset[3] = DOTILE(3,{0.2},false);
    tileset[4] = DOTILE(4,{0.2},true);
    tileset[5] = DOTILE(5,{0.2},true);
    tileset[6] = DOTILE(6,{0.2},true);
    tileset[7] = DOTILE(7,{0.2},true);
    tileset[8] = DOTILE(8,{0.2},false);
    tileset[9] = DOTILE(9,{0.2},true);
}
  Tile create_tile(int tilenr,float time_offset = -1){
    Tile out = tileset[tilenr];
    if(time_offset > 0){
      out.time_offset = time_offset;
    }else{
      out.time_offset = (float)(rand() % 100) / 10.0;
    }
    return out;
  }

};

class ObjectCreator{
public:
  game_object game_objects[100];
  ObjectCreator(){

  }

  game_object * create_game_object(int game_object_number,bool & physical){
    if(game_object_number == 0){
      physical = true;
      return new Cow();
    }
  }
  
};






class game_editor:public EventListener<KeyEvent>, public EventListener<MouseClick>,public EventListener<mouse_position>{
public:
  TileCreator tc;
  ObjectCreator oc;
  ObjectHandler * object_handler;
  game_editor(ObjectHandler * objh){
    object_handler = objh;
  }

  void handle_event(MouseClick mc){
    if(mc.pressed == true && mc.button == 0){
      mouse_position mpos = get_mouse_position();
      mouse_position world_pos = screen_pos_to_world_pos(mpos);
      create_tile(0,world_pos.x,world_pos.y);
    }
    if(mc.pressed == true && mc.button == 1){
      mouse_position mpos = get_mouse_position();
      mouse_position world_pos = screen_pos_to_world_pos(mpos);
      create_object(0,world_pos.x,world_pos.y);
    } 
  }

  void  handle_event(mouse_position mp){

  }

  void handle_event(KeyEvent kev){

  }

  void create_object(int object_nr,float x, float y){
    bool physical;
    game_object * ngo = oc.create_game_object(0,physical);
    ngo->x = x;
    ngo->y = y;
    if(physical){
      object_handler->load_object((physical_game_object *) ngo);
    }else{
      object_handler->load_object(ngo);
    }
  }

  void create_tile(int tile_nr, float x, float y){
    object_handler->tile_map.set_tile((x + (int) x%18)/18,(y + (int)y%10)/10,tc.create_tile(4));
  }

};


int main(){


  init_game(1024,1024,256,256);
  
  set_clearcolor(0.1,0.5,0.0,1.0);
  GLProgram ptest = texture_shader;
  
  ObjectHandler object_handler;
  game_editor ge(&object_handler);
  object_handler.current_shader = ptest;

  Music m1("ko-ko.ogg");
  Music m2(m1);
  AudioSample boom("boom.ogg");
 // play_music(m2);
  
  Texture fb_tex = make_texture((void *) NULL,global_screen_width,global_screen_height,3);  
  FrameBuffer fb(fb_tex);
  
  Texture treetex = make_texture("cow.png");
  Texture guy_tex  = make_texture("DormusSheet.png");
  BufferObject bobj = unit_rectangle_verts;
  BufferObject bobj2 = unit_rectangle_uvs;
  BufferObject bobj3 = unit_rectangle_inverse_uvs;
 
 SpriteSheetDrawable treeTD= SpriteSheetDrawable(treetex); 
  treeTD.load_animation_frame("test",48,30,0,0,0.2);
  treeTD.load_animation_frame("test",48,30,48,0,2.0);
  treeTD.set_animation("test");

  Texture tree= make_texture("tree123.png");
  SpriteSheetDrawable tree_sprite(tree);
  tree_sprite.load_animation_frame("def",tree.width,tree.height,0,0,1000);
  tree_sprite.set_animation("def");

  object_handler.load_object(make_pgo(200,0,10,0,0,20,false,false,tree_sprite));
  object_handler.load_object(make_pgo(300,0,10,0,0,20,false,false,tree_sprite));
  object_handler.load_object(make_pgo(200,50,10,0,0,20,false,false,tree_sprite));
  object_handler.load_object(make_pgo(300,50,10,0,0,20,false,false,tree_sprite));


  Texture air=make_texture("air_elemental.png");

  SpriteSheetDrawable air_sprite(air);
  air_sprite.load_animation_frame("idle",20,20,0,0,0.2);
  air_sprite.load_animation_frame("idle",20,20,20,0,0.2);
  air_sprite.load_animation_frame("idle",20,20,40,0,0.2);
  air_sprite.load_animation_frame("idle",20,20,60,0,0.2);
  air_sprite.set_animation("idle");
  object_handler.load_object(make_pgo(100,100,20,5,0,5,true,true,air_sprite));

  player_object dormus(14,10,10,10,0,7,true,false,guy_tex);
  player_object * a=&dormus; 
  key_event_handler.register_listener(a);
  mouse_click_handler.register_listener(&ge);
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
    //** Go gameloop!
    object_handler.gameloop();
    //
    

  }
  
  return 0;
}
