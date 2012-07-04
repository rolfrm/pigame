#include "tilemap.h"

tilemap::tilemap(){

  }
tilemap::tilemap(int sizex, int sizey, Tile _default){
  size_x = sizex;
  size_y = sizey;
  def = _default;
  for(int i = 0; i < size_x*size_y;i++){
    data.push_back(def);
  }
  
}
Tile tilemap::get_tile(int x, int y){
  if(x >= size_x || y >= size_y){
    return def;
  }
  if(x < 0 || y < 0){
    return def;
  }
  return data[x + size_x*y];
}
void tilemap::set_tile(int x, int y, Tile value){
  if(x >= size_x || y >= size_y){
    return;
  }
  if(x < 0 || y < 0){
    return;
  }
  data[x + size_x*y] = value;
}
Tile& tilemap::tile(int x, int y){
  return data[x + size_x*y];
}
void tilemap::get_tilechunk(int from_x, float to_x, float from_y, float to_y, Tile * data_out){
  
}

Tile::Tile(bool passable, SpriteSheetDrawable *  ssd,int tile_nr,float time){
  this->passable = passable;
  this->tile_nr = tile_nr;
  sprite_sheet = ssd;
  if(time >= 0){
    time_offset = time;
  }else{
    time_offset = ((float)(rand() % 100))/100;
  }
}
Tile::Tile(){
  sprite_sheet = NULL;
}
void Tile::handle_collision(physical_game_object * obj){

}
