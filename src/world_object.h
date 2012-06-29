#pragma once

#include<list>
class ObjectHandler;
class game_object;
class physical_game_object;
class WorldObject{
 ObjectHandler * object_handler;
 std::list<game_object *> remove_list;
 public:
  WorldObject(ObjectHandler * object_handler);
 
  std::list <game_object *> get_near_gameobjects(game_object * origin, float distance);
  std::list <physical_game_object *> get_near_physical_objects(physical_game_object * origin, float distance);
  
  void remove_object(game_object *);
  void insert_object(game_object *);
  void insert_object(physical_game_object *);
  
  std::list <physical_game_object *>  get_objects_nearby_to_point(float x, float y, float distance);
  void finish_update();
};
