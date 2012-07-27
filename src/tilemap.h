#pragma once
#include <vector>
#include <iostream>
#include "drawable.h"
#include "game_objects.h"

class Tile{
 public:
  Tile(bool passable ,int tile_nr,float time_offset = -1);
  Tile();
  virtual void handle_collision(physical_game_object * obj);  
  bool passable;
  float time_offset;
  int tile_nr;
};

class tilemap{
  std::vector<Tile> data;
  int size_x;
  int size_y;
  Tile def;
 public:
  tilemap();
  tilemap(int sizex, int sizey, Tile _default);
  Tile get_tile(int x, int y);
  void set_tile(int x, int y, Tile value);
  Tile& tile(int x, int y);
  void get_tilechunk(int from_x, float to_x, float from_y, float to_y, Tile * data_out);

};
