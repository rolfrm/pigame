#include "game.h"
#include <iostream>
#include <math.h>
std::list <game_object *> WorldObject::get_near_gameobjects(game_object * origin, float distance){
  //Not implemented
  std::cout << "Warning: get_near_gameobjects not implemented\n";
  std::list<game_object *> out;
  /*AABB a = origin->get_aabb();
  for(std::list<game_object *>::iterator it = object_handler->drawlist.begin();it++){
    if(*it == origin){
      continue;
    }
    AABB b = it->get_aabb();
    }*/
  return out;
}
float aabb_distance(AABB a, AABB b){
  float dx = fabs(a.x - b.x) - a.size_x - b.size_x;
  float dy = fabs(a.y - b.y) - a.size_y - b.size_y;
  float out;
  if(dx < dy){
    out = dy;
  }else{
    out = dx;
  }
  //std::cout << "Distance " << dx << " " << dy <<" =" << out <<  "\n";
  
  if(out < 0){
    out = 0;
  }
  
  return out;
}
std::list <physical_game_object *> WorldObject::get_near_physical_objects(physical_game_object * origin, float distance){
  std::list<physical_game_object *> out;
  AABB a = origin->get_aabb();
  for(std::list<physical_game_object *>::iterator it = object_handler->physical_sim.begin(); it != object_handler->physical_sim.end();it++){
    if(*it == origin){
      continue;
    }
    AABB b = (*it)->get_aabb();
    float d = aabb_distance(a,b);
    if(d < distance){
      out.push_back(*it);
    }
  }
  return out;
}

WorldObject::WorldObject(ObjectHandler * object_handler){
  this->object_handler = object_handler;
}




void ObjectHandler::gameloop(){
  WorldObject wo(this);
    for(std::list<game_object *>::iterator it = drawlist.begin(); it != drawlist.end() ;it++){
      game_object * gobj = *it;
      gobj->do_ai(wo);
    }
    
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

    std::list<Drawable*> render_list;
    for(std::list<game_object *>::iterator it = drawlist.begin(); it != drawlist.end(); it++){
      render_list.push_back((*it)->draw());
    }
    render_list.sort(z_compare);
    
    for(std::list<Drawable *>::iterator it = render_list.begin();it != render_list.end(); it++){
      (*it)->draw(current_shader);
    }

    
  }
