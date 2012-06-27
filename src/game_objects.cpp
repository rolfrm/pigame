#include "game_objects.h"
#include "game_super.h"
#include <iostream>

AABB game_object::get_aabb(){
  AABB out;
  out.x = x;
  out.y = y;
  out.size_x = 1;
  out.size_y = 1;
}

void player_object::handle_event(mouse_position mpos){
  mpos = screen_pos_to_world_pos(mpos);
  x = (x*4 + mpos.x)/5;
  y = (y*4 + mpos.y)/5;
}

player_object::player_object(){
  down = false;
}
void player_object::handle_event(MouseClick mc){
  if(mc.pressed){
    down = true;

    mouse_position mpos = get_mouse_position();
    mpos = screen_pos_to_world_pos(mpos);
    //x = mpos.x;
    //y = mpos.y;
  }else{
    down = false;
  }
}

void player_object::do_ai( WorldObject wo){
  if(down){
    set_camera_position(x,y);
    std::cout << wo.get_near_physical_objects(this,180.0).size() << "\n";
  }
}

void physical_game_object::set_aabb_data(float size_x, float size_y, float co_x, float co_y, bool movable, bool ghost){
  aabb.size_x = size_x;
  aabb.size_y = size_y;
  aabb.movable = movable;
  aabb.ghost = ghost;
  this->co_x = co_x;
  this->co_y = co_y;
}

void physical_game_object::handle_collision(physical_game_object * other){
  std::cout << "Collision happened\n";
}

AABB physical_game_object::get_aabb(){
  AABB ab = aabb;
  ab.x = x + co_x;
  ab.y = y + co_y;
  return ab;
}

void physical_game_object::set_aabb(AABB ab){
  x = ab.x - co_x;
  y = ab.y - co_y;
}

Person::Person(){
  max_speed = 1.5;
  set_aabb_data(30,15,0,0,true);
}


Drawable * game_object::draw(){
  tex_draw.x = x;
  tex_draw.y = y;
  tex_draw.z = -y;
  return &tex_draw;
}

