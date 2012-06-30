#pragma once
#include <list>
#include "game_objects.h"
#include "GLProgram.h"
#include "world_object.h"
#include "tilemap.h"
#include "frame_buffer.h"
#include "game_super.h"

class Tile{
 public:
  Tile(bool passable,SpriteSheetDrawable * ss);
  Tile();
  virtual void handle_collision(physical_game_object * obj);
  SpriteSheetDrawable * sprite_sheet;
  bool passable;
};

class ObjectHandler{
  friend class WorldObject;
  tilemap <Tile> tile_map;
  FrameBuffer * sprite_rendering_buffer;
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
