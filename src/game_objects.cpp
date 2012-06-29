#include "game_objects.h"
#include "game_super.h"
#include <iostream>
#include <math.h>
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
  down = 0;
  up = 0;
  left = 0;
  right = 0;
}

player_object::player_object(float x,float y,float sx, float sy, float off_x, float off_y,bool movable, bool ghost,Texture sheet){
  down = 0;
  up = 0;
  left = 0;
  right = 0;	

  tex_draw=SpriteSheetDrawable(sheet); 
  tex_draw.load_animation_frame("rwalk",20,20,0,0,0.2);
  tex_draw.load_animation_frame("rwalk",20,20,20,0,0.2);
  tex_draw.load_animation_frame("rwalk",20,20,0,0,0.2);
  tex_draw.load_animation_frame("rwalk",20,20,40,0,0.2);

  tex_draw.load_animation_frame("dwalk",20,20,0,20,0.2);
  tex_draw.load_animation_frame("dwalk",20,20,40,20,0.2);
  tex_draw.load_animation_frame("dwalk",20,20,20,20,0.2);
  tex_draw.load_animation_frame("dwalk",20,20,40,20,0.2);

  tex_draw.load_animation_frame("uwalk",20,20,0,40,0.2);
  tex_draw.load_animation_frame("uwalk",20,20,40,40,0.2);
  tex_draw.load_animation_frame("uwalk",20,20,20,40,0.2);
  tex_draw.load_animation_frame("uwalk",20,20,40,40,0.2);

  tex_draw.load_animation_frame("lwalk",20,20,0,60,0.2);
  tex_draw.load_animation_frame("lwalk",20,20,20,60,0.2);
  tex_draw.load_animation_frame("lwalk",20,20,0,60,0.2);
  tex_draw.load_animation_frame("lwalk",20,20,40,60,0.2);
  tex_draw.set_animation("lwalk");

  this->x=x;
  this->y=y;
  set_aabb_data(sx,sy,off_x,off_y,movable,ghost);

}


void player_object::handle_event(MouseClick mc){
  if(mc.pressed){
    down = true;

    mouse_position mpos = get_mouse_position();
    mpos = screen_pos_to_world_pos(mpos);
    std::cout << x << " " << y << "\n";
    //x = mpos.x;
    //y = mpos.y;
  }else{
    down = false;
  }
}

void player_object::handle_event(KeyEvent kev){
  int active = kev.pressed;
  switch(kev.key){
  case 283:up=active;break; //Up
    case 285:left=active;break; //left
    case 284:down=active;break; //down
    case 286:right=active;break; //right
    }
}

void player_object::do_ai( WorldObject wo){
  x += left*1 + right*-1;
  y += up*-1 + down;
  set_camera_position(x,y);

	if(right){
		if(tex_draw.current_animation->first!="rwalk")
			tex_draw.set_animation("rwalk");
	} 
	else if(left){
		if(tex_draw.current_animation->first!="lwalk")
			tex_draw.set_animation("lwalk");
	}
	else if(up){
		if(tex_draw.current_animation->first!="uwalk")
			tex_draw.set_animation("uwalk");
	}
	else if(down){
		if(tex_draw.current_animation->first!="dwalk")
			tex_draw.set_animation("dwalk");
	}

  if(down){
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
  //std::cout << "Collision happened\n";
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


SpriteSheetDrawable * game_object::draw(){
  tex_draw.x = x;
  tex_draw.y = y;
  tex_draw.z = -y;
  return &tex_draw;
}

Bullet::Bullet(float size_x,float size_y,float co_x,float co_y,Texture tex){
	set_aabb_data(size_x,size_y,co_x,co_y,true,false);
	x=co_x;
	y=co_y;
	dead=false;
}

void Bullet::do_ai(WorldObject wo){
	if(dead){
		wo.remove_object(this);
	}
	else{
		x+=vel[0],y+=vel[1];
	}


}

