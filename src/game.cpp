#include "game.h"
int global_screen_width;
int global_screen_height;

extern unsigned int global_width;
extern unsigned int global_height;
mouse_position screen_pos_to_world_pos(mouse_position mpos){
  
  mpos.x = global_width/2 - mpos.x;
  mpos.y = mpos.y - global_height/2;
  mpos.x = mpos.x * (int)global_screen_width*2;
  mpos.y = mpos.y * (int)global_screen_height*2;
  mpos.x /= (int)global_width;
  mpos.y /= (int)global_height;
  return mpos;
}
	
