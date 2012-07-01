#include "game.h"
#include "game_super.h"
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include "gfx_basics.h"
#include <initializer_list>

void ObjectHandler::update_ai(){
  WorldObject wo(this);
  std::list<game_object *> drawlist2 = drawlist;
  for(std::list<game_object *>::iterator it = drawlist2.begin(); it != drawlist2.end() ;it++){
    game_object * gobj = *it;
    gobj->do_ai(wo);
  }
  wo.finish_update();  
}


ObjectHandler::ObjectHandler(){

  sprite_rendering_buffer=new FrameBuffer(global_screen_width,global_screen_height,4);

  Texture tex = make_texture("grass_tiles.png");
  TileSpriteFactory tsf("grass_tiles.png",18,10);
  SpriteSheetDrawable * ssdtest1 = tsf.make_animated_tile(0,{0.2,0.2}); 
  SpriteSheetDrawable * ssd = tsf.make_animated_tile(0,{0.2}); 
  
  SpriteSheetDrawable * ssd2 = tsf.make_animated_tile(1,{1.0,2.0});
  SpriteSheetDrawable * ssd3 = tsf.make_animated_tile(6,{0.2});
  SpriteSheetDrawable * ssd4 = tsf.make_animated_tile(7,{0.2});
  SpriteSheetDrawable * ssds[4];
  ssds[0] = ssd;
  ssds[1] = ssd2;
  ssds[2] = ssd3;
  ssds[3] = ssd4;
  
  SpriteSheetDrawable * water = tsf.make_animated_tile(2,{0.2,0.2});
  SpriteSheetDrawable * water2 = tsf.make_animated_tile(8,{1.0,1.0});
  SpriteSheetDrawable * wood = tsf.make_animated_tile(3,{0.2});
  SpriteSheetDrawable * wood2 = tsf.make_animated_tile(4,{0.2});
  SpriteSheetDrawable * sand = tsf.make_animated_tile(5,{3.0,3.0,3.0});
  
  
  tile_map = tilemap(20,20,Tile(false,water));
  
  for(int i = 0; i < 20;i++){
    for(int j = 0; j < 20;j++){
      tile_map.set_tile(i,j,Tile(true,ssds[1 + rand()%3],(float)i / 10.0));
    }
  }

  
  for(int i = 0; i < 20;i++){
    tile_map.set_tile(i,0,Tile(true,sand,rand()%100));
    tile_map.set_tile(i,1,Tile(true,sand,rand()%100));
    tile_map.set_tile(i,2,Tile(true,sand,rand()%100));
    //tile_map.set_tile(i,3,Tile(true,sand));
    
    tile_map.set_tile(0,i,Tile(true,sand,rand()%100));
    tile_map.set_tile(19,i,Tile(true,sand,rand()%100));
    tile_map.set_tile(i,19,Tile(true,sand,rand()%100));
  
    }

  for(int i = 0; i < 10;i++){
    tile_map.set_tile(i,0,Tile(false,water,(float)i/20.0));
    tile_map.set_tile(i,1,Tile(false,water,(float)i/20.0));
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

void ObjectHandler::update_physics(){

  std::list<collision_pair> collision_pairs;
    for(std::list<physical_game_object *>::iterator it = physical_sim.begin(); it !=physical_sim.end();it++){
      physical_game_object * p1 = *it;
      AABB aabb1 = p1->get_aabb(); 
      if(!aabb1.ghost && aabb1.movable){
	for(int i = -aabb1.size_x/18 - 1;i < aabb1.size_x/18+1;i++){
	  for(int j = -aabb1.size_y/10-1;j < aabb1.size_y/10+1;j++){
	    
	    Tile tile = tile_map.get_tile(aabb1.x/18 + i, aabb1.y/10 + j);
	    if(tile.passable == false){
	      
	      AABB ab2;
	      ab2.x = aabb1.x - (int) aabb1.x%18 + i*18;
	      ab2.y = aabb1.y - (int) aabb1.y%10 + j*10;
	      ab2.size_x = 18/2*0.8;
	      ab2.size_y = 10/2*0.8;
	      float overlap;
	      int result = AABB_collision_detection2(&aabb1, &ab2, overlap);
	      if(result >= 0){
		if(result == 0){
		  aabb1.x += overlap*0.7;
		}else{
		  aabb1.y += overlap*0.7;
		}
	      }
	    }
	  }
	}
      }
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
    for(std::list<collision_pair>::iterator it = collision_pairs.begin(); it != collision_pairs.end(); it++){
      (*it).pgo1->handle_collision((*it).pgo2);
      (*it).pgo2->handle_collision((*it).pgo1);
    
    }
}

bool y_order_compare(DrawRequest dr1, DrawRequest dr2){
  return dr1.y < dr2.y;
}

#include "misc.h"
void ObjectHandler::do_rendering(){
  std::list<DrawRequest> drs,tile_drs;

  bind_framebuffer(*sprite_rendering_buffer);
  clear_bound_framebuffer();
  
  for(std::list<game_object *>::iterator it = drawlist.begin(); it != drawlist.end(); it++){
    drs.push_back((*it)->draw());
  }
  drs.sort(y_order_compare);
  
  bind_shader(texture_shader);
  setup_shader_uniforms(texture_shader);
  bind_buffer_object(unit_rectangle_verts,0);
  bind_buffer_object(unit_rectangle_uvs,1);

  float t = get_time();
  for(float x = camera_x-200; x < camera_x + 200;x+=18){
    for(float y = camera_y-200; y < camera_y + 200;y+=10){
      Tile tile = tile_map.get_tile(x/18 , y/10);
      SpriteSheetDrawable * ssd = tile.sprite_sheet; 
      DrawRequest dr;
      if(tile.time_offset > 0.01){
	dr = ssd->special_draw_request(0,0,t+tile.time_offset);
      }else{
	dr = ssd->make_draw_request();
      }
      dr.x = x - (int)x%18;
      dr.y = y - (int)y%10;
      tile_drs.push_back(dr);
    }
  }

  for(std::list<DrawRequest>::iterator it = tile_drs.begin();it != tile_drs.end();it++){
    DrawRequest dr = *it;
    bind_texture(dr.tex,0);
    texture_shader.uniformf("object_scale",dr.vert_scale_x,dr.vert_scale_y);
    texture_shader.uniformf("off",dr.x,dr.y);
    texture_shader.uniformf("uv_scale",dr.uv_scale_x,dr.uv_scale_y);
    texture_shader.uniformf("uv_offset",dr.uv_off_x, dr.uv_off_y);
    draw_buffers_triangle_fan(4);

  }

  bind_shader(shadow_drawer);
  setup_shader_uniforms(shadow_drawer);
  shadow_drawer.uniformf("lDirection",-0.5,-0.5);
  for(std::list<DrawRequest>::iterator it = drs.begin();it != drs.end();it++){
    DrawRequest dr = *it;
    bind_texture(dr.tex,0);	
    shadow_drawer.uniformf("object_scale",dr.vert_scale_x,dr.vert_scale_y);
    shadow_drawer.uniformf("off",dr.x,dr.y);
    shadow_drawer.uniformf("uv_scale",dr.uv_scale_x,dr.uv_scale_y);
    shadow_drawer.uniformf("uv_offset",dr.uv_off_x, dr.uv_off_y);
    draw_buffers_triangle_fan(4);

  }
  bind_shader(texture_shader);
  for(std::list<DrawRequest>::iterator it = drs.begin();it != drs.end();it++){
    DrawRequest dr = *it;
    bind_texture(dr.tex,0);
    texture_shader.uniformf("object_scale",dr.vert_scale_x,dr.vert_scale_y);
    texture_shader.uniformf("off",dr.x,dr.y);
    texture_shader.uniformf("uv_scale",dr.uv_scale_x,dr.uv_scale_y);
    texture_shader.uniformf("uv_offset",dr.uv_off_x, dr.uv_off_y);
    draw_buffers_triangle_fan(4);

  }
  unbind_framebuffer();

  bind_shader(screen_drawer);
  bind_buffer_object(unit_rectangle_inverse_uvs,1);
  screen_drawer.uniformf("camera",0,0);
  screen_drawer.uniformf("scale",1.0,1.0);
  screen_drawer.uniformf("object_scale",1.0,1.0);
  screen_drawer.uniformf("off",0,0);
  bind_texture(sprite_rendering_buffer->textures[0],0);

  draw_buffers_triangle_fan(4);
    
  swapbuffers();

}


void ObjectHandler::gameloop(){
  update_ai();
  update_physics();
  do_rendering();
    
  }
void ObjectHandler::load_object(physical_game_object * pobj){
    physical_sim.push_back(pobj);
    drawlist.push_back(pobj);
  }

void ObjectHandler::load_object(game_object * gobj){
  drawlist.push_back(gobj);
}
