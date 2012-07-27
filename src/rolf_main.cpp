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
    this->tex = tex;
    this->current_animation = Animation(animation_frames);
    
    set_aabb_data(width,height,offset_x, offset_y, movable,ghost);
  }

  game_object * clone(){
    return new NoAIGameObject(*this);
  }
};

#include <functional>
class SimpleAIGameObject: public physical_game_object{
public:
  std::function<void (physical_game_object*, WorldObject & wo)> ai_func;
  std::function<void (physical_game_object*, physical_game_object *)> collision_func;
 
  SimpleAIGameObject(Texture tex, std::vector<frame> animation_frames, float width, float height, float offset_x, float offset_y, bool movable, bool ghost){
    this->tex = tex;
    this->current_animation = Animation(animation_frames);
    
    set_aabb_data(width,height,offset_x, offset_y, movable,ghost);
  }

  
  game_object * clone(){
    return new SimpleAIGameObject(*this);
  }
  void do_ai(WorldObject & wo){
    if(ai_func){
      ai_func(this,wo);
    }
  }
  void handle_collision(physical_game_object * other){
    if(collision_func){
      collision_func(this,other);
    }
  }
  
};


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
#include<algorithm>

bool isPlayer(physical_game_object * pg){
  return pg->id == 2;
}
physical_game_object * Bug(){
  SimpleAIGameObject * nago;
  nago = new SimpleAIGameObject(make_texture("sprites/sprites1.png"),
			{frame(10,8,0,0,0.1),frame(10,8,20,0,0.1)},
			3,3,0,3,true,false);			
  nago->ai_func = [](physical_game_object * self, WorldObject wo){
    std::list<physical_game_object *> plist= wo.get_near_physical_objects(self,25.0);
    auto it = find_if(plist.begin(), plist.end(), isPlayer);
    if(it == plist.end()){
      return;
    }
    physical_game_object * player = *it;
    
    float dx = (player->x - self->x);
    float dy = (player->y - self->y);
    float lx = fabs(dx);
    float ly = fabs(dy);
    if(lx > 10.0){
      self->x +=dx/lx;
    }

    if(ly > 10.0){
      self->y +=dy/ly;
    }
  };
  return nago;
}

NoAIGameObject * Scroll(){
  Texture tex = make_texture("sprites/scroll.png");
  return new NoAIGameObject(tex,{frame(23,27,0,0,0.2),frame(23,27,23,0,0.2),frame(23,27,46,0,0.2)},3,3,0,3,true,false);
  
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
  std::vector<MasterTile> tileset;
  TileCreator(){
    
    //my first c++ lambda function :p
    Texture tex = get_texture("grass_tiles.png");
    auto DOTILE = [&tileset,&tex](int nr, std::vector<float> timings, bool passable){
      std::vector<frame> frames;
      for(int i = 0; i < timings.size();i++){
	frames.push_back(frame(18,10,i*18,10*nr,timings[i]));
      }


      tileset.push_back(MasterTile(Animation(frames),tex,passable));
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
    DOTILE(20,{0.2,0.2,0.2,0.2,0.2,0.2,0.2},true);
    DOTILE(21,{0.2},true);
    DOTILE(22,{0.2},true);
    DOTILE(23,{0.2,0.2,0.2,0.2,0.2,0.2,0.2},false);
    DOTILE(24,{0.2,0.2,0.2,0.2},true);
    DOTILE(25,{0.2,0.2,0.2,0.2},true);
    DOTILE(26,{0.2,0.2,0.2,0.2},true);
  }
  Tile create_tile(int tilenr,float time_offset = -1){
    Tile out(tileset[tilenr].passable,tilenr,time_offset);
    if(time_offset > 0){
      out.time_offset = 0;//time_offset;
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
  
public:
  tile_view():UIElement(20,-10){}
  void set_tile(MasterTile mt){
    anim = mt.animation;
    tex = mt.texture;
  }
  
};

class object_view: public UIElement{
  game_object * go;
public:
  object_view():UIElement(512,-5){}
  void set_object(game_object * ngo){
    go = ngo;
    anim = ngo->current_animation;
    tex = ngo->tex;
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
    object_handler->master_tiles = tc.tileset;
    current_tile = 0;
    current_game_object = 0;
    tw = new tile_view();
    ow = new object_view();
    bool physical;
    ow->set_object(oc.create_game_object(current_game_object,physical));
    tw->set_tile(tc.tileset[current_tile]);
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
    if(kev.key == 289 && kev.pressed){
      current_tile = (current_tile+1) % tc.tileset.size();
      tw->set_tile(tc.tileset[current_tile]);
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
