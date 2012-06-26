#pragma once
#include <list>
#include "game_objects.h"
#include "GLProgram.h"

class ObjectHandler{
 public:
  std::list<game_object * > drawlist;
  std::list<physical_game_object *> physical_sim;
  GLProgram current_shader;
  void load_object(game_object * gobj){
    drawlist.push_back(gobj);
  }

  void load_object(physical_game_object * pobj){
    physical_sim.push_back(pobj);
    drawlist.push_back(pobj);
  }

  void gameloop();
};
