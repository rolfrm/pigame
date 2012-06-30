#include "game.h"
#include "game_super.h"
#include <iostream>
#include <math.h>
#include <stdlib.h>

Tile::Tile(bool passable, SpriteSheetDrawable *  ssd){
  this->passable = passable;
  sprite_sheet = ssd;
}
Tile::Tile(){
  sprite_sheet = NULL;
}
void Tile::handle_collision(physical_game_object * obj){

}

void ObjectHandler::UpdateAI(){
  WorldObject wo(this);
  std::list<game_object *> drawlist2 = drawlist;
  for(std::list<game_object *>::iterator it = drawlist2.begin(); it != drawlist2.end() ;it++){
    game_object * gobj = *it;
    gobj->do_ai(wo);
  }
  wo.finish_update();
  
}

ObjectHandler::ObjectHandler(){


  Texture tex = make_texture("grass_tiles.png");
  SpriteSheetDrawable * ssd = new SpriteSheetDrawable(tex);
  SpriteSheetDrawable * ssd2 = new SpriteSheetDrawable(tex);
  SpriteSheetDrawable * ssd3 = new SpriteSheetDrawable(tex);
  SpriteSheetDrawable * ssd4 = new SpriteSheetDrawable(tex);
  SpriteSheetDrawable * ssds[4];
  ssds[0] = ssd;
  ssds[1] = ssd2;
  ssds[2] = ssd3;
  ssds[3] = ssd4;
  ssd->load_animation_frame("1",18,10,0,0,0.2);
  ssd2->load_animation_frame("1",18,10,18,0,0.2);
  ssd3->load_animation_frame("1",18,10,0,10,0.2);
  ssd4->load_animation_frame("1",18,10,18,10,0.2);
  
  SpriteSheetDrawable * water = new SpriteSheetDrawable(tex);
  water->load_animation_frame("1",18,10,0,20,0.2);
  water->load_animation_frame("1",18,10,18,20,0.2);
  water->set_animation("1");

  
  SpriteSheetDrawable * wood = new SpriteSheetDrawable(tex);
  wood->load_animation_frame("1",18,10,18,30,0.2);
  wood->set_animation("1"); 
  
  SpriteSheetDrawable * wood2 = new SpriteSheetDrawable(tex);
  wood2->load_animation_frame("1",18,10,0,30,0.2);
  wood2->set_animation("1"); 
  
  SpriteSheetDrawable * sand = new SpriteSheetDrawable(tex);
  sand->load_animation_frame("1",18,10,36,30,0.2);
  sand->set_animation("1"); 
  


  /*
  ssd->load_animation_frame("1",18,10,0,0,0.2);
  ssd->load_animation_frame("1",18,10,18,0,0.2);
  ssd->load_animation_frame("1",18,10,36,0,0.2);
  ssd->load_animation_frame("1",18,10,54,0,0.2);
  
  ssd2->load_animation_frame("1",18,10,0,10,0.2);
  ssd2->load_animation_frame("1",18,10,18,10,0.2);
  ssd2->load_animation_frame("1",18,10,36,10,0.2);
  ssd2->load_animation_frame("1",18,10,54,10,0.2);

  ssd3->load_animation_frame("1",18,10,0,20,0.2);
  ssd3->load_animation_frame("1",18,10,18,20,0.2);
  ssd3->load_animation_frame("1",18,10,36,20,0.2);
  ssd3->load_animation_frame("1",18,10,54,20,0.2);

  ssd4->load_animation_frame("1",18,10,0,30,0.2);
  ssd4->load_animation_frame("1",18,10,18,30,0.2);
  ssd4->load_animation_frame("1",18,10,36,30,0.2);
  ssd4->load_animation_frame("1",18,10,54,30,0.2);
  */
  ssd->set_animation("1");
  ssd2->set_animation("1");
  ssd3->set_animation("1");
  ssd4->set_animation("1");

  
  tile_map = tilemap<Tile>(20,20,Tile(false,water));
  
  for(int i = 0; i < 20;i++){
    for(int j = 0; j < 20;j++){
      tile_map.set_tile(i,j,Tile(true,ssds[1 + rand()%3]));
    }
  }

  
  for(int i = 0; i < 20;i++){
    tile_map.set_tile(i,0,Tile(true,sand));
    tile_map.set_tile(i,1,Tile(true,sand));
    tile_map.set_tile(i,2,Tile(true,sand));
    //tile_map.set_tile(i,3,Tile(true,sand));
    
    tile_map.set_tile(0,i,Tile(true,sand));
    tile_map.set_tile(19,i,Tile(true,sand));
    tile_map.set_tile(i,19,Tile(true,sand));
  
    }

  for(int i = 0; i < 10;i++){
    tile_map.set_tile(i,0,Tile(false,water));
    tile_map.set_tile(i,1,Tile(false,water));
  }

  tile_map.set_tile(2,0,Tile(true,wood));
  tile_map.set_tile(2,1,Tile(true,wood));

  for(int i = 0; i < 6; i++){
    tile_map.set_tile(10 + i,10,Tile(false,wood));
    tile_map.set_tile(10 + i,15,Tile(false,wood));
    tile_map.set_tile(10,10+i,Tile(false,wood));
    tile_map.set_tile(15,10+i,Tile(false,wood));
    
  }
  for(int i = 0; i < 4;i++){
    for(int j = 0; j < 4;j++){
      tile_map.set_tile(11 + i,11 + j,Tile(true,wood2));
    
    }
  }
  tile_map.set_tile(14,15,Tile(true,wood2));
    
}

struct collision_pair{
  physical_game_object * pgo1, * pgo2;
};

void ObjectHandler::UpdatePhysics(){

  std::list<collision_pair> collision_pairs;
    for(std::list<physical_game_object *>::iterator it = physical_sim.begin(); it !=physical_sim.end();it++){
      physical_game_object * p1 = *it;
      AABB aabb1 = p1->get_aabb(); 
      for(int i = -aabb1.size_x/18 - 1;i < aabb1.size_x/18+1;i++){
	for(int j = -aabb1.size_y/10-1;j < aabb1.size_y/10+1;j++){

	  Tile tile = tile_map.get_tile(aabb1.x/18 + i, aabb1.y/10 + j);
	  if(tile.passable == false){
	    
	    AABB ab2;
	    ab2.x = aabb1.x - (int) aabb1.x%18 + i*18;
	    ab2.y = aabb1.y - (int) aabb1.y%10 + j*10;
	    ab2.size_x = 18/2*0.9;
	    ab2.size_y = 10/2*0.9;
	    float overlap;
	    int result = AABB_collision_detection2(&aabb1, &ab2, overlap);
	    if(result >= 0){
	      if(result == 0){
		aabb1.x += overlap*0.5;
	      }else{
		aabb1.y += overlap*0.5;
	      }
	    }
	  }
	}
      }
      //std::cout << tile.passable << "\n";
      std::list<physical_game_object *>::iterator it2 = it;
      it2++;
      for(; it2 != physical_sim.end();it2++){
	physical_game_object * p2 = *it2;
	AABB aabb2 = p2->get_aabb();
	if(aabb2.movable == false && aabb1.movable == false){
	  continue;
	}
	float overlap;

	int result = AABB_collision_detection2(&aabb1, &aabb2, overlap);
	
	float w1 = 0.5;
	float w2 = 0.5;
	if(aabb1.movable == false){
	  w2 = 1.0;
	  w1 = 0;
	}else if(aabb2.movable == false){
	  w1 = 1.0;
	  w2 = 0;
	}

	if(result == -1){
	  continue;
	}
	collision_pair cp = {p1,p2};
	collision_pairs.push_back(cp);
	if(aabb1.ghost == true || aabb2.ghost == true){
	  overlap = 0.0;
	}

	if(result == 0){
	  aabb1.x += overlap*w1;
	  aabb2.x -= overlap*w2;
	}else if(result == 1){
	  aabb1.y += overlap*w1;
	  aabb2.y -= overlap*w2;
	}
	p2->set_aabb(aabb2);
	
      }
      p1->set_aabb(aabb1);
    }
    //std::list<collision_pair> collision_pairs;
    for(std::list<collision_pair>::iterator it = collision_pairs.begin(); it != collision_pairs.end(); it++){
      (*it).pgo1->handle_collision((*it).pgo2);
      (*it).pgo2->handle_collision((*it).pgo1);
    
    }
}

void ObjectHandler::DoRendering(){

  
  
  std::list< SpriteSheetDrawable *> render_list;
  for(std::list<game_object *>::iterator it = drawlist.begin(); it != drawlist.end(); it++){
    render_list.push_back((*it)->draw());
  }
  render_list.sort(z_compare);
  
  bind_shader(texture_shader);
  setup_shader_uniforms(texture_shader);
  bind_buffer_object(unit_rectangle_verts,0);
  bind_buffer_object(unit_rectangle_uvs,1);

  std::list<DrawRequest> drs;
  std::cout << camera_x << "\n";
  for(float x = camera_x-100; x < camera_x + 100;x+=18){
    for(float y = camera_y-100; y < camera_y + 100;y+=10){
      SpriteSheetDrawable * ssd = tile_map.get_tile(x/18 , y/10).sprite_sheet; 
      DrawRequest dr = ssd->MakeDrawRequest();
      dr.x = x - (int)x%18;
      dr.y = y - (int)y%10;
      drs.push_back(dr);
    }
    }

  
  for(std::list<SpriteSheetDrawable *>::iterator it = render_list.begin();it != render_list.end(); it++){
    DrawRequest dr = (*it)->MakeDrawRequest();
    drs.push_back(dr);
  }
  for(std::list<DrawRequest>::iterator it = drs.begin();it != drs.end();it++){
    DrawRequest dr = *it;
    bind_texture(dr.tex,0);
    texture_shader.uniformf("object_scale",dr.vert_scale_x,dr.vert_scale_y);
    texture_shader.uniformf("off",dr.x,dr.y);
    texture_shader.uniformf("uv_scale",dr.uv_scale_x,dr.uv_scale_y);
    texture_shader.uniformf("uv_offset",dr.uv_off_x, dr.uv_off_y);
    draw_buffers_triangle_fan(4);
  

  }

}


void ObjectHandler::gameloop(){
  UpdateAI();
  UpdatePhysics();
  DoRendering();
    
  }
void ObjectHandler::load_object(physical_game_object * pobj){
    physical_sim.push_back(pobj);
    drawlist.push_back(pobj);
  }

void ObjectHandler::load_object(game_object * gobj){
  drawlist.push_back(gobj);
}
