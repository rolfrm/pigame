#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <vector>
#include <list>
#include <math.h>
#include <iostream>
#include <fstream>
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

class NoAIGameObject: public physical_game_object{
public:
  NoAIGameObject(Texture tex, std::vector<frame> animation_frames, float width, float height, float offset_x, float offset_y, bool movable, bool ghost){
    tex_draw = SpriteSheetDrawable(tex);
    tex_draw.load_animation_frames("main_animation", animation_frames);
    tex_draw.set_animation("main_animation");
    set_aabb_data(width,height,offset_x, offset_y, movable,ghost);
  }

  NoAIGameObject(SpriteSheetDrawable ssd, float width, float height, float offset_x, float offset_y, bool movable, bool ghost){
    tex_draw = ssd;
    set_aabb_data(width,height,offset_x, offset_y, movable,ghost);
  }
  game_object * clone(){
    return new NoAIGameObject(*this);
  }
};

/*class SimpleAIGameObject: public physical_game_object{
public:
  SimpleAIGameObject(SpriteSheetDrawable ssd, float width, float height, float offset_x, float offset_y, bool movable, bool ghost){
    tex_draw = ssd;
    tex_draw.load_animation_frames("main_animation", animation_frames);
    tex_draw.set_animation("main_animation");
    set_aabb_data(width,height,offset_x, offset_y, movable,ghost);
  }
  game_object * clone(){
    return new SimpleAIGameObject(*this);
  }
  };*/


AABB make_aabb(float x, float y, float size_x, float size_y,bool movable, bool ghost){
  AABB out = {x,y,size_x,size_y,movable,ghost};
  return out;
	
}

NoAIGameObject * Cow(){
  return new NoAIGameObject(make_texture("cow.png"),
			{frame(48,30,0,0,0.2),frame(48,30,48,0,0.2)},
			20,10,0,5,true,false);			
}

NoAIGameObject * Tree(){
  Texture tex = make_texture("sprites/tree1.png");
  return new NoAIGameObject(tex,
			{frame(tex.width,tex.height,0,0,0.2)},
			10,10,0,5,false,false);			
}

NoAIGameObject * Well(){
  Texture tex = make_texture("sprites/well.png");
  return new NoAIGameObject(tex,
			{frame(tex.width,tex.height,0,0,0.2)},
			11,5,0,8,false,false);	
}

NoAIGameObject * Bug(){
  return new NoAIGameObject(make_texture("sprites/sprites1.png"),
			{frame(10,8,0,0,0.1),frame(10,8,20,0,0.1)},
			3,3,0,3,true,false);			
}

NoAIGameObject * Scroll(){
  return new NoAIGameObject(SpriteSheetDrawable::from_file("ssd_test"),
			3,3,0,3,true,false);
  /*
  return new NoAIGameObject(make_texture("sprites/scroll.png"),
			{frame(23,27,0,0,0.2),frame(23,27,23,0,0.2),frame(23,27,46,0,0.2)},
			3,3,0,3,true,false);			*/
}




class key_ev: public EventListener<KeyEvent>{
  bool handle_event(KeyEvent kev){
    switch(kev.key){
    case 283:set_camera_position(camera_x,camera_y - 1);break; //Up
    case 285:set_camera_position(camera_x + 1,camera_y );break; //left
    case 284:set_camera_position(camera_x,camera_y + 1 );break; //down
    case 286:set_camera_position(camera_x - 1,camera_y );break; //right
    }
    return true;
  }
};

class TileCreator{
public:
  Tile tileset[100];
  TileCreator(){
    TileSpriteFactory tsf("grass_tiles.png", 18,10);

    //my first c++ lambda function :p
    auto DOTILE = [this,&tsf](int nr, std::vector<float> timings, bool passable){
      tileset[nr] = Tile(passable,tsf.make_animated_tile(nr,timings),nr);
    };
    DOTILE(0,{0.2},false);
    
    DOTILE(1,{0.2},true);
    DOTILE(2,{0.2,0.2},true);
    DOTILE(3,{0.2,0.2,0.2,0.2},false);
    DOTILE(4,{0.2},false);
    DOTILE(5,{0.2},true);
    DOTILE(6,{0.2},true);
    DOTILE(7,{0.2},true);
    DOTILE(8,{0.2},true);
    DOTILE(9,{0.2},false);
    DOTILE(10,{0.2},true);
    DOTILE(11,{0.2},true);
    DOTILE(12,{0.2},true);
    DOTILE(13,{0.2},true);
    DOTILE(14,{0.2},true);
    DOTILE(15,{0.2},true);
    DOTILE(16,{0.2},true);
    DOTILE(17,{0.2},true);
    DOTILE(18,{0.2},true);
    DOTILE(19,{0.2},true);
    
    tileset[20] = Tile(true,tsf.make_animated_tile(20,{0.2,0.2,0.2,0.2,0.2,0.2,0.2}),20);
    DOTILE(21,{0.2},true);
    DOTILE(22,{0.2},true);
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
  struct object_container{
    game_object * go;
    bool physical;
  };
  std::vector<object_container> prototypes;
  
  ObjectCreator(){
    
#define PROTO(_type,physical) prototypes.push_back(object_container({_type(), physical}))

    PROTO(Cow,true);
    PROTO(Tree,true);
    PROTO(Well,true);
    PROTO(Bug,true);
    PROTO(Scroll,true);
#undef PROTO
  }

  game_object * create_game_object(int game_object_number,bool & physical){
    
    object_container oc = prototypes[game_object_number];
    physical = oc.physical;
    return oc.go->clone();
  }
  
};

class tile_view: public UIElement{
  Tile current_tile;
public:
  tile_view():UIElement(20,-10){}
  void set_tile(Tile t){
    current_tile = t;
    drawable = *t.sprite_sheet;
  }
  
};

class object_view: public UIElement{
  game_object * go;
public:
  object_view():UIElement(512,-5){}
  void set_object(game_object * ngo){
    go = ngo;
    drawable = go->tex_draw;
  }
};

class object_selection: public UIElement{
public:
  object_selection():UIElement(0,0){}
  void set_object(game_object * focus){
    

  }
};


class game_editor:public EventListener<KeyEvent>, public EventListener<MouseClick>,public EventListener<mouse_position>{
public:
  struct tile_event{
    unsigned char tile;
    unsigned short x,y;
  };

  struct object_event{
    unsigned char object;
    unsigned short x,y;
  };

  TileCreator tc;
  ObjectCreator oc;
  ObjectHandler * object_handler;
  bool running;
  tile_view * tw;
  object_view * ow;
  int current_tile;
  int current_game_object;
  std::vector<tile_event> tile_events;
  std::vector<object_event> object_events;
  game_editor(){
    running = true;
    object_handler = new ObjectHandler(tc.create_tile(0));
    current_tile = 0;
    current_game_object = 0;
    tw = new tile_view();
    ow = new object_view();
    bool physical;
    ow->set_object(oc.create_game_object(current_game_object,physical));
    tw->set_tile(tc.create_tile(current_tile));
    object_handler->load_ui_element(tw);
    object_handler->load_ui_element(ow);
    load_game("test.map","test.objects");
  }
  bool handle_event(MouseClick mc){
    if(mc.pressed == true && mc.button == 0){
      mouse_position mpos = get_mouse_position();
      mouse_position world_pos = screen_pos_to_world_pos(mpos);
      create_tile(current_tile,world_pos.x,world_pos.y);
    }
    if(mc.pressed == true && mc.button == 1){
      mouse_position mpos = get_mouse_position();
      mouse_position world_pos = screen_pos_to_world_pos(mpos);
      create_object(current_game_object, world_pos.x,world_pos.y);
    } 
    if(mc.button == 2){
      running = false;
    }
    return true;
  }

  bool  handle_event(mouse_position mp){
    return true;
  }

  bool handle_event(KeyEvent kev){
    std::cout << kev.key << "\n";
    if(kev.key == 289 && kev.pressed){
      current_tile++;
      current_tile = current_tile % 23;
      tw->set_tile(tc.create_tile(current_tile));
    }else if(kev.key == 287 && kev.pressed){
      current_game_object = (current_game_object + 1)%oc.prototypes.size();
      bool physical;
      ow->set_object(oc.prototypes[current_game_object].go);
    
    }
    return true;
  }

  void create_object(int object_nr,float x, float y){
    object_event oe = {(unsigned char) object_nr, (unsigned short) x, (unsigned short) y};
    object_events.push_back(oe);

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
    tile_event te = {(unsigned char) tile_nr,(unsigned short) x, (unsigned short) y};
    tile_events.push_back(te);
    object_handler->tile_map.set_tile((x + (int) x%18)/18,(y + (int)y%10)/10,tc.create_tile(tile_nr));
  }
  void save_game(std::string tile_path,std::string object_path){
    char * write_from = (char *) &(tile_events[0]);
    std::ofstream file;
    file.open(tile_path,std::ios::binary | std::ios::out);
    if(file.good()){
      file.write(write_from,sizeof(tile_event)*tile_events.size());
    }
    
    file.close();
    write_from =(char *) &(object_events[0]);
    file.open(object_path,std::ios::binary | std::ios::out);
    if(file.good()){
      file.write(write_from,sizeof(object_event)*object_events.size());
    }
    file.close();
  }

  void load_game(std::string tile_path, std::string object_path){
    std::ifstream file;
    tile_event te;
    file.open(tile_path,std::ios::binary | std::ios::in);
    while(file.good()){
      file.read((char *)&te,sizeof(tile_event));
      create_tile(te.tile,te.x,te.y);
    }
    file.close();

    object_event oe;
    file.open(object_path,std::ios::binary | std::ios::in);
    while(file.good()){
      file.read((char *)&oe,sizeof(object_event));
      create_object(oe.object,oe.x,oe.y);
    }
    file.close();


  }

};


int main(){
  

  init_game(512,512,128,128);
  /*SpriteSheetDrawable::from_file("ssd_test");
    return 0;*/
  set_clearcolor(0.1,0.5,0.0,1.0);
  GLProgram ptest = texture_shader;
  
  //ObjectHandler object_handler;
  game_editor ge;
  ge.object_handler->current_shader = ptest;

  /*Music m1("ko-ko.ogg");
  Music m2(m1);
  AudioSample boom("boom.ogg");
  */
  player_object dormus(7,15,8,5,0,7,true,false);
  player_object * a=&dormus; 
  key_event_handler.register_listener(a);
  mouse_click_handler.register_listener(&ge);
  key_event_handler.register_listener(&ge);
  ge.object_handler->load_object(a);
  

  double start_t = get_time();
  while(ge.running){
    double next_t = get_time();
    double dt = next_t - start_t;
    double dt_sleep = 1.0/60.0 - dt;
    start_t = next_t;
    if(dt_sleep > 0){
      sleep_sec(dt_sleep);
      start_t += dt_sleep;
    }
    ge.object_handler->gameloop();

  }
  ge.save_game("test.map","test.objects");
  
  return 0;
}
