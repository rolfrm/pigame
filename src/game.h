#pragma once
#include <list>
#include "game_objects.h"
#include "GLProgram.h"
#include "world_object.h"
#include "tilemap.h"
#include "frame_buffer.h"
#include "game_super.h"

class TileSpriteFactory{
public:
  /*Assumes tiles are arrenged as rows*/
  Texture tex;
  int tile_width;
  int tile_height;
  TileSpriteFactory(std::string texture_path, int tile_width, int tile_height){
    tex = make_texture(texture_path.c_str());
    this->tile_width = tile_width;
    this->tile_height = tile_height;
  }
  SpriteSheetDrawable * make_animated_tile(int row,std::vector<float> timings){
    SpriteSheetDrawable * ssd = new SpriteSheetDrawable(tex);
    for(int i = 0; i < timings.size();i++){
      ssd->load_animation_frame("1",tile_width,tile_height,i*tile_width,tile_height*row,timings[i]);
    }
    ssd->set_animation("1");
    return ssd;
  }

};

class UIElement:public Drawer, public EventListener<MouseClick>{
 public:
  float x, y;
  SpriteSheetDrawable drawable;
  UIElement(float _x, float _y){
    x = _x;
    y = _y;
  }
  DrawRequest draw(){
    return drawable.make_draw_request(x,y);
  }
  bool handle_event(MouseClick){ return true;}

};


class ObjectHandler{
  friend class WorldObject;
  
  FrameBuffer * sprite_rendering_buffer;
 public:
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
