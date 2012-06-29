#pragma once
#include "drawable.h"
#include "event_handling.h"
#include "input_events.h"
#include "CollisionDetection.h"

class ObjectHandler;
class game_object;
class physical_game_object;
class WorldObject{
 ObjectHandler * object_handler;
 
 public:
  WorldObject(ObjectHandler * object_handler);
 
  std::list <game_object *> get_near_gameobjects(game_object * origin, float distance);
  std::list <physical_game_object *> get_near_physical_objects(physical_game_object * origin, float distance);
  
};

class game_object:public Drawer{
public:
  float x,y,dx,dy;
 game_object(){
    x = 0;
    y = 0.0;
    dx = 0.15;
    dy = 0.15;
  }
   
  SpriteSheetDrawable tex_draw;
 
  SpriteSheetDrawable * draw();
  virtual void do_ai(WorldObject wo){

  }

  virtual AABB get_aabb();
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


class player_object: public Person, public EventListener<mouse_position>, public EventListener<MouseClick>, public EventListener<KeyEvent>{
  int down;
  int up;
  int left;
  int right;
public:
  void handle_event(mouse_position mpos);
  void handle_event(MouseClick mc);
  void handle_event(KeyEvent ke);
  
  void do_ai(WorldObject wo);
  player_object();
  player_object(float x,float y,float sx, float sy, float off_x, float off_y,bool movable, bool ghost,Texture sheet);
  
};
