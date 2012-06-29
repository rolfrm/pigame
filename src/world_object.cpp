#include "world_object.h"
#include "game_objects.h"
#include "game.h"
#include <iostream>
#include <math.h>

std::list <game_object *> WorldObject::get_near_gameobjects(game_object * origin, float distance){
  //Not implemented
  std::cout << "Warning: get_near_gameobjects not implemented\n";
  std::list<game_object *> out;
  /*AABB a = origin->get_aabb();
  for(std::list<game_object *>::iterator it = object_handler->drawlist.begin();it++){
    if(*it == origin){
      continue;
    }
    AABB b = it->get_aabb();
    }*/
  return out;
}

float aabb_distance(AABB a, AABB b){
  float dx = fabs(a.x - b.x) - a.size_x - b.size_x;
  float dy = fabs(a.y - b.y) - a.size_y - b.size_y;
  float out;
  if(dx < dy){
    out = dy;
  }else{
    out = dx;
  }
  
  if(out < 0){
    out = 0;
  }
  
  return out;
}
std::list <physical_game_object *> WorldObject::get_near_physical_objects(physical_game_object * origin, float distance){
  std::list<physical_game_object *> out;
  AABB a = origin->get_aabb();
  for(std::list<physical_game_object *>::iterator it = object_handler->physical_sim.begin(); it != object_handler->physical_sim.end();it++){
    if(*it == origin){
      continue;
    }
    AABB b = (*it)->get_aabb();
    float d = aabb_distance(a,b);
    if(d < distance){
      out.push_back(*it);
    }
  }
  return out;
}

WorldObject::WorldObject(ObjectHandler * object_handler){
  this->object_handler = object_handler;
}

void WorldObject::remove_object(game_object * obj){
  remove_list.push_back(obj);
}

void WorldObject::insert_object(game_object * new_object){

}

void WorldObject::insert_object(physical_game_object * obj){
    remove_list.push_back(obj);

}

void WorldObject::finish_update(){
  for(std::list<game_object *>::iterator it = remove_list.begin(); it != remove_list.end();it++){
    std::cout << "Removing object .. \n";
    object_handler->drawlist.remove((game_object *) *it);
    object_handler->physical_sim.remove((physical_game_object *) *it);
  }
  remove_list.clear();
}


std::list <physical_game_object *>  get_objects_nearby_to_point(float x, float y, float distance){

}
