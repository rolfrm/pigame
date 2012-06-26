#pragma once
#include "drawable.h"
#include "event_handling.h"
#include "input_events.h"
#include "CollisionDetection.h"

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
  
  TextureDrawable tex_draw;
 
  Drawable * draw();
  virtual void do_ai(){

  }
};

class physical_game_object: public game_object{
  float co_x, co_y;
  AABB aabb;
 public:
  void set_aabb_data(float size_x, float size_y, float co_x, float co_y, bool collidable,bool ghost = false);
  AABB get_aabb();
  void set_aabb(AABB naabb);
  void handle_collision(physical_game_object * other);
};

class Creature: public physical_game_object{
 public:
  int level;
  float max_speed;

};

class Person: public Creature{

 public:
  Person();
};

class Npc: public Person{

};


class player_object: public Person, public EventListener<mouse_position>{
public:
  void handle_event(mouse_position mpos);
  void do_ai();
  
};
