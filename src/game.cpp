#include "game.h"
#include "game_super.h"
#include <iostream>
#include <math.h>



void ObjectHandler::UpdateAI(){
  WorldObject wo(this);
  for(std::list<game_object *>::iterator it = drawlist.begin(); it != drawlist.end() ;it++){
    game_object * gobj = *it;
    gobj->do_ai(wo);
  }
  std::cout << "Finished ai\n";
  wo.finish_update();
  
}

void ObjectHandler::UpdatePhysics(){

    for(std::list<physical_game_object *>::iterator it = physical_sim.begin(); it !=physical_sim.end();it++){
      physical_game_object * p1 = *it;
      AABB aabb1 = p1->get_aabb(); 
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
	if(aabb1.ghost == true || aabb2.ghost == true){
	  overlap = 0.0;
	}
	if(result >= 0){
	  p1->handle_collision(p2);
	  p2->handle_collision(p1);
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
  
  for(std::list<SpriteSheetDrawable *>::iterator it = render_list.begin();it != render_list.end(); it++){
    DrawRequest dr = (*it)->MakeDrawRequest();
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
