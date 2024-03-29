#pragma once
#include "drawable.h"
#include "event_handling.h"
#include "input_events.h"
#include "CollisionDetection.h"
#include "world_object.h"

class game_object:public Drawer{
public:
  int id;
  float x,y,dx,dy;
 game_object(){
   id = 0;
    x = 0;
    y = 0.0;
    dx = 0.15;
    dy = 0.15;
  }
   
  Animation current_animation;
  Texture tex;
  DrawRequest draw();
  virtual void do_ai(WorldObject& wo){

  }

  virtual game_object * clone()=0;

  virtual AABB get_aabb();
};

class physical_game_object: public game_object{
  float co_x, co_y;
  public:
  AABB aabb;
 
  void set_aabb_data(float size_x, float size_y, float co_x, float co_y, bool movable,bool ghost = false);
  AABB get_aabb();
  void set_aabb(AABB naabb);
  virtual void handle_collision(physical_game_object * other);
};

class Bullet:public physical_game_object{
public:
	Bullet(int x,int y,float co_x,float co_y,int vel_x,int vel_y);

	void do_ai(WorldObject & wo);
	void handle_collision(physical_game_object * other);

	float vel[2];
	double start_time;
	bool dead;
	game_object * clone(){
	  return new Bullet(*this);
	};
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
  Animation uwalk,dwalk,rwalk,lwalk;
  
  int down;
  int up;
  int left;
  int right;
  int latest_key;
  int orientation[2];
  game_object * collider;
  bool spawn_bullet;
  Texture bullet_tex;
public:
  void print_frames();
  bool handle_event(mouse_position mpos);
  bool handle_event(MouseClick mc);
  bool handle_event(KeyEvent ke);
  void handle_collision(physical_game_object * other);
  void do_ai(WorldObject & wo);
  player_object();
  player_object(float x,float y,float sx, float sy, float off_x, float off_y,bool movable, bool ghost);
  game_object * clone(){
    return new player_object(*this);
  };
  
};
