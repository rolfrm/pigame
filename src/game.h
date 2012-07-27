#pragma once
#include <list>
#include "game_objects.h"
#include "GLProgram.h"
#include "world_object.h"
#include "tilemap.h"
#include "frame_buffer.h"
#include "game_super.h"

class MasterTile{
 public:
  Animation animation;
  Texture texture;
  bool passable;
  MasterTile(Animation animation, Texture tex,bool passable);
  MasterTile();

};

class UIElement:public Drawer, public EventListener<MouseClick>{
 public:
  float x, y;
  Animation anim;
  Texture tex;
  UIElement(float _x, float _y){
    x = _x;
    y = _y;
  }
  DrawRequest draw(){
    return anim.make_draw_request(tex,x,y);
  }
  bool handle_event(MouseClick){ return true;}

};


class ObjectHandler{
  friend class WorldObject;
  
  FrameBuffer * sprite_rendering_buffer;
 public:
  std::vector<MasterTile> master_tiles;
  std::list<UIElement *> uilist;
  tilemap tile_map;
  ObjectHandler(Tile default_tile);
  std::list<game_object * > drawlist;
  std::list<physical_game_object *> physical_sim;
  GLProgram current_shader;
  void load_ui_element(UIElement * ui_el);
  void load_object(game_object * gobj);
  void load_object(physical_game_object * pobj);
  void update_ai();
  void update_physics();
  void do_rendering();
  void gameloop();
};
