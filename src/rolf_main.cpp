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
      ssd.load_animation_frames("test",{frame(48,30,0,0,0.2),frame(48,30,48,0,0.2)});
      ssd.set_animation("test");
      inited = 1;
    }
    tex_draw = ssd;
    set_aabb_data(20,10,0,5,true,false);
  }
};
SpriteSheetDrawable Cow::ssd;
int Cow::inited = -1;

class Tree: public physical_game_object{
public:
  static SpriteSheetDrawable ssd;
  static int inited;
  Tree(){ 
    if(inited == -1){
      Texture tex = make_texture("sprites/tree1.png");
      ssd = SpriteSheetDrawable(tex);
      ssd.load_animation_frames("test",{frame(tex.width,tex.height,0,0,0.9)});
      ssd.set_animation("test");
      inited = 1;
    }
    tex_draw = ssd;
    set_aabb_data(10,10,0,5,false,false);
  }
};
SpriteSheetDrawable Tree::ssd;
int Tree::inited = -1;

class Well: public physical_game_object{
public:
  static SpriteSheetDrawable ssd;
  static int inited;
  Well(){ 
    if(inited == -1){
      Texture tex = make_texture("sprites/well.png");
      ssd = SpriteSheetDrawable(tex);
      ssd.load_animation_frames("test",{frame(tex.width,tex.height,0,0,0.9)});
      ssd.set_animation("test");
      inited = 1;
    }
    tex_draw = ssd;
    set_aabb_data(11,5,0,8,false,false);
  }
};
SpriteSheetDrawable Well::ssd;
int Well::inited = -1;

class Bug: public physical_game_object{
public:
  static SpriteSheetDrawable ssd;
  static int inited;
  Bug(){ 
    if(inited == -1){
      Texture tex = make_texture("sprites/sprites1.png");
      ssd = SpriteSheetDrawable(tex);
      ssd.load_animation_frames("walk",{frame(10,8,0,0,0.1),frame(10,8,20,0,0.1)});
      ssd.set_animation("walk");
      inited = 1;
    }
    tex_draw = ssd;
    set_aabb_data(3,3,0,3,true,false);
  }
};
SpriteSheetDrawable Bug::ssd;
int Bug::inited = -1;

class Scroll: public physical_game_object{
public:
  static SpriteSheetDrawable ssd;
  static int inited;
  Scroll(){ 
    if(inited == -1){
      Texture tex = make_texture("sprites/scroll.png");
      ssd = SpriteSheetDrawable(tex);
      ssd.load_animation_frames("walk",{frame(23,27,0,0,0.2),frame(23,27,23,0,0.2),frame(23,27,46,0,0.2)});
      ssd.set_animation("walk");
      inited = 1;
    }
    tex_draw = ssd;
    set_aabb_data(3,3,0,3,true,false);
  }
};
SpriteSheetDrawable Scroll::ssd;
int Scroll::inited = -1;


player_object * make_player_obj(float x,float y,float sx, float sy, float off_x, float off_y,bool movable, bool ghost, SpriteSheetDrawable tex_draw ){
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
      return new Cow();
    }else if(game_object_number == 1){
      physical = true;
      return new Tree();
    }else if(game_object_number == 2){
      physical = true;
      return new Well();
    }else if(game_object_number == 3){
      physical = true;
      return new Bug();
    }else if(game_object_number == 4){
      physical = true;
      return new Scroll();
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
      create_tile(3,world_pos.x,world_pos.y);
    }
    if(mc.pressed == true && mc.button == 1){
      mouse_position mpos = get_mouse_position();
      mouse_position world_pos = screen_pos_to_world_pos(mpos);
      create_object(rand()%5,world_pos.x,world_pos.y);
    } 
  }

  void  handle_event(mouse_position mp){

  }

  void handle_event(KeyEvent kev){

  }

  void create_object(int object_nr,float x, float y){
    bool physical;
    game_object * ngo = oc.create_game_object(object_nr,physical);
    ngo->x = x;
    ngo->y = y;
    if(physical){
      object_handler->load_object((physical_game_object *) ngo);
    }else{
      object_handler->load_object(ngo);
    }
  }

  void create_tile(int tile_nr, float x, float y){
    object_handler->tile_map.set_tile((x + (int) x%18)/18,(y + (int)y%10)/10,tc.create_tile(tile_nr));
  }

};


int main(){
  

  init_game(512,512,128,128);
  
  set_clearcolor(0.1,0.5,0.0,1.0);
  GLProgram ptest = texture_shader;
  
  ObjectHandler object_handler;
  game_editor ge(&object_handler);
  object_handler.current_shader = ptest;

  Music m1("ko-ko.ogg");
  Music m2(m1);
  AudioSample boom("boom.ogg");

  player_object dormus(7,15,8,5,0,7,true,false);
  player_object * a=&dormus; 
  key_event_handler.register_listener(a);
  mouse_click_handler.register_listener(&ge);
  object_handler.load_object(a);
  

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
    object_handler.gameloop();

  }
  
  return 0;
}
