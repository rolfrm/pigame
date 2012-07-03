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


ObjectHandler::ObjectHandler(Tile default_tile){

  sprite_rendering_buffer=new FrameBuffer(global_screen_width,global_screen_height,4);

  
  tile_map = tilemap(2000,2000,default_tile);
      
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

  texture_shader.uniformf("camera",256,-256);
  texture_shader.uniformf("scale",2.0/global_width,2.0/global_height);
  bind_shader(texture_shader);
  for(std::list<UIElement *>::iterator it = uilist.begin();it != uilist.end();it++){
    DrawRequest dr = (*it)->draw();
    bind_texture(dr.tex,0);
    texture_shader.uniformf("object_scale",dr.vert_scale_x,dr.vert_scale_y);
    texture_shader.uniformf("off",dr.x - dr.vert_scale_x,dr.y - dr.vert_scale_y);
    texture_shader.uniformf("uv_scale",dr.uv_scale_x,dr.uv_scale_y);
    texture_shader.uniformf("uv_offset",dr.uv_off_x, dr.uv_off_y);
    draw_buffers_triangle_fan(4);

  }


    
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
void ObjectHandler::load_ui_element(UIElement * ui_el){
  uilist.push_back(ui_el);
}
