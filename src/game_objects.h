#pragma once
#include "drawable.h"
#include "event_handling.h"
#include "game.h"
class game_object:public Drawer{
public:
  float x,y,dx,dy;
  int wx,wy;
  game_object(){
    x = 0;
    y = 0.0;
    dx = 0.15;
    dy = 0.15;
    wx = 10;
    wy = 10;
  }
  
  virtual void do_ai(){
   
   }
};

class player_object: public game_object, public EventListener<mouse_position>{
public:
  TextureDrawable tdraw;
  void handle_event(mouse_position mpos){
    mpos = screen_pos_to_world_pos(mpos);
    x = mpos.x;
    y = mpos.y;
  }

  void do_ai(){
   
    x += 1;
    if(x > 180){
      x = - 180;
    }
  }
  Drawable * draw(){
    tdraw.x = x;
    tdraw.y = y;
    tdraw.z = -y;
    return &tdraw;
  }
};
