#pragma once
#include <list>
#include "game_objects.h"
#include "GLProgram.h"
#include "world_object.h"
#include "tilemap.h"

class ObjectHandler{
  friend class WorldObject;
  tilemap <SpriteSheetDrawable*> tile_map;
 public:
  ObjectHandler();
  std::list<game_object * > drawlist;
  std::list<physical_game_object *> physical_sim;
  GLProgram current_shader;
  void load_object(game_object * gobj);
  void load_object(physical_game_object * pobj);
  void UpdateAI();
  void UpdatePhysics();
  void DoRendering();
  void gameloop();
};
