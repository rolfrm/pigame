#include "game_objects.h"
#include "game_super.h"
#include "misc.h"
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
  collider = NULL;
}

player_object::player_object(float x,float y,float sx, float sy, float off_x, float off_y,bool movable, bool ghost){
  down = 0;
  up = 0;
  left = 0;
  right = 0;	

  Texture sheet=make_texture("DormusSheet.png");

  tex_draw=SpriteSheetDrawable(sheet); 
<<<<<<< HEAD
  tex_draw.load_animation_frame("rwalk",20,20,0,80,0.1);
  tex_draw.load_animation_frame("rwalk",20,20,20,80,0.1);
  tex_draw.load_animation_frame("rwalk",20,20,0,80,0.1);
  tex_draw.load_animation_frame("rwalk",20,20,40,80,0.1);

  tex_draw.load_animation_frame("dwalk",20,20,0,20,0.1);
  tex_draw.load_animation_frame("dwalk",20,20,40,20,0.1);
  tex_draw.load_animation_frame("dwalk",20,20,20,20,0.1);
  tex_draw.load_animation_frame("dwalk",20,20,40,20,0.1);

  tex_draw.load_animation_frame("uwalk",20,20,0,40,0.1);
  tex_draw.load_animation_frame("uwalk",20,20,40,40,0.1);
  tex_draw.load_animation_frame("uwalk",20,20,20,40,0.1);
  tex_draw.load_animation_frame("uwalk",20,20,40,40,0.1);

  tex_draw.load_animation_frame("lwalk",20,20,0,100,0.1);
  tex_draw.load_animation_frame("lwalk",20,20,20,100,0.1);
  tex_draw.load_animation_frame("lwalk",20,20,0,100,0.1);
  tex_draw.load_animation_frame("lwalk",20,20,40,100,0.1);
  tex_draw.set_animation("lwalk");

  this->x=x;
  this->y=y;
  set_aabb_data(sx,sy,off_x,off_y,movable,ghost);

  spawn_bullet=false;

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
  int button = kev.key;
  std::cout << "event :" << button << " " << active << "\n";
  switch(button){
  case 283:up=active;break; //Up
  case 285:left=active;break; //left
  case 284:down=active;break; //down
  case 286:right=active;break; //right
  case 'W':up=active;break;
  case 'A':left=active;break; //left
  case 'S':down=active;break; //down
  case 'D':right=active;break; //right
  
  case 32:spawn_bullet=kev.pressed;break; //space
  case 294:if(active){aabb.ghost = !aabb.ghost;}break;
    }
}

void player_object::handle_collision(physical_game_object * pgo){
  
}

void player_object::do_ai( WorldObject & wo){
  
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


  if(spawn_bullet){
	spawn_bullet=false;
	wo.insert_object(new Bullet(x+20,y,0,0,1,0,get_texture("bullet.png")));
	
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


DrawRequest game_object::draw(){
 
  DrawRequest dr = tex_draw.make_draw_request(x,y);
  return dr;
}

Bullet::Bullet(int x,int y,float co_x,float co_y,int vel_x,int vel_y,Texture tex){
	set_aabb_data(tex.width/2,tex.height/2,co_x,co_y,true,true);
	this->x=x;
	this->y=y;
	dead=false;
	tex_draw=SpriteSheetDrawable(tex);
 	tex_draw.load_animation_frame("static",tex.width,tex.height,0,0,200000);
	tex_draw.set_animation("static");
	vel[0]=vel_x;
	vel[1]=vel_y;
	start_time=get_time();
	
}

void Bullet::do_ai(WorldObject & wo){
	if(get_time()-start_time>5){
		dead=true;
	}
	

	if(dead){
		wo.remove_object(this);
	}
	else{
		x+=vel[0];
		y+=vel[1];
	}


}

void Bullet::handle_collision(physical_game_object * other){
	other->x+=vel[0]*10,other->y+=vel[1]*10;
	vel[0]=-vel[0];
	//dead=true;

}

