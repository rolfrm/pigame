#include <vector>
#include <list>
#include <math.h>
#include <iostream>
#include "CollisionDetection.h"

#include "texture.h"
#include "GLProgram.h"
#include "buffer_objects.h"

#include "event_handling.h"
#include "input_events.h"
#include "audio.h"
#include "gfx_basics.h"
#include "misc.h"
#include <functional>
#include <algorithm>
#include "shared_ptr.h"
#include <FTGL/ftgl.h>
GLProgram naive_shader;

struct Color{
  float r,g,b,a;
};

class World;

class GameObject{
public:
  GameObject(float _x, float _y, float _sx,float _sy, Color _col){
    x = _x;
    y = _y;
    sx = _sx;
    sy = _sy;
    col = _col;
    clickable = true;
  }

  virtual void do_ai(shared_ptr<GameObject> self, World * wld){
    if(ai){
      ai(self,wld);
    }
  }

  GameObject(){}
  float x,y;
  float sx,sy;
  Color col;
  bool clickable;
  std::function<void(shared_ptr<GameObject> self, World * wld)> ai;

};

typedef shared_ptr<GameObject> GameObject_ref;


class World{
public:
  World(){
  }
  
  std::vector<shared_ptr<GameObject> > game_objects;
  std::list<unsigned int> remove_list;
  std::list<GameObject_ref> add_list;
  void remove_object(shared_ptr<GameObject> go){
    for(int i = 0; i < game_objects.size();i++){
      if(game_objects[i].data == go.data){
	remove_list.push_back(i);
      }
    }
  }
  void add_object(GameObject_ref go){
    std::cout << "Adding: " << go->x << " " << go->y << " " << go->sx << " " << go->sy << "\n";
    add_list.push_back(go);
  }
  void update(){
    remove_list.sort();
    remove_list.reverse();
  
    for(std::list<unsigned int>::iterator it = remove_list.begin();it != remove_list.end();it++){
      game_objects.erase(game_objects.begin() +*it);
      }
    
    remove_list.clear();
    for(std::list<GameObject_ref>::iterator it = add_list.begin();it != add_list.end();it++){
      game_objects.push_back(*it);
    }
    add_list.clear();
  }
  std::list<shared_ptr<GameObject>> get_game_objects_at(float x, float y){
    std::list<shared_ptr<GameObject>> gos;
    std::for_each(game_objects.begin(),game_objects.end(),
		  [&gos,x,y](shared_ptr<GameObject> go){
		    
		    if(go->clickable){
		      float dx = x - go->x;
		      float dy = -y - go->y;
		      
		      if (fabs(dx) < go->sx/2 && fabs(dy) < go->sy/2){
			gos.push_back(go);
		      }
		    }
		  }
		  );
      return gos;
  }
  
};

BufferObject square_vbo;

class Level{
public:
  World * wld;
  float score;
  
  Level(World *_wld){
    wld = _wld;
    score = 0;
  }

  virtual void draw_backdrop(){}
  virtual void draw_game_objects(std::vector<GameObject> &gos){}
  virtual void update(){}
  virtual void draw_foreground(){}
  virtual void on_clicked_object(GameObject_ref go){}
};

 float randf(){
    return (float)(rand()%1000)/1000.0;
  }


  float dx = 0.001;

  void gof2 (shared_ptr<GameObject> self, World *wld){
    self->col.a *=0.9;
    self->sx *= 0.90;
    self->sy *= 0.90;
  if(self->col.a < 0.01){
    wld->remove_object(self);
  }
}


  void gof (shared_ptr<GameObject> self, World *wld){
    if (self->x > 1.0 || self->x < -1.0){
      dx = -0.9*dx;
      wld->remove_object(self);
      
      GameObject ngo = GameObject(0.5*(randf()-0.5),0.5*(randf()-0.5),randf()*0.2+ 0.05,randf()*0.2+ 0.05,{randf(),randf(),randf(),1.0});
      ngo.ai = gof;
      wld->add_object(GameObject_ref(ngo));
	}

    self->x += dx;
}

std::function<void(GameObject_ref, World *)> gofer(){
  float dx = 0.05*(randf() - 0.5);
  float dy = 0.05*(randf() - 0.5);
  float t = get_time();
  auto lambd = [t,dx,dy](GameObject_ref self, World * wld){
    self->clickable = true;
    self->x += dx;
    self->y += dy;
    if(get_time() - t > 1){
      self->ai = gof2;
    }
  };
  return lambd;

}



  
  std::function<void(GameObject_ref, World *)> zoomin_func(GameObject_ref copy){
    float t = get_time();
    float csx = copy->sx;
    float csy = copy->sy;
  auto lambd = [=](GameObject_ref self, World * wld){
    self->clickable = true;
    float dt = (get_time() - t);
    float scale = dt;
    self->sx = csx*scale;
    self->sy = csy*scale;
    if(dt > 1){
      self->sx = copy->sx;
      self->sy = copy->sy;
      self->ai = gofer();
    }
  };
  return lambd;

}

AudioSample test_proc_snd();
class Level1: public Level{

public:
  GLProgram lv1_shader;
  float t_start;
  float t;
  float next;
  float it;
  float next_x;
  float next_y;
  GameObject_ref next_go;
  FTGLPixmapFont font;
  Music musics;
  AudioSample as;
  Level1(World *_wld):Level(_wld),next_go(new GameObject(0,0,0.5,0.5,{1.0,0,0,1.0})),font("/usr/share/fonts/truetype/msttcorefonts/Arial.ttf"),musics("ko-ko.ogg"), as("sound/jaja.ogg"){
    lv1_shader = make_program("shaders/naive.vert","shaders/naive.frag");
    //play_music(musics);
    play_audio_sample(&as,0);
    t_start = get_time();
    next = 0;
    next_x = 0.0;
    next_y = 0.0;
    it = 1;

    next_go = GameObject(randf()*2.0 - 1.0, randf()*2.0 - 1.0 ,randf()*0.2+ 0.05,randf()*0.2+ 0.05,{randf(),randf(),randf(),1.0});
  }
  void draw_backdrop(){
    bind_shader(lv1_shader);
    lv1_shader.uniformf("size",2.0,2.0);
    lv1_shader.uniformf("pos",0.0,0.0);
    lv1_shader.uniformf("color",0.2,0.5,0.0,0.5);
    bind_buffer_object(square_vbo,0);
    draw_buffers_triangle_fan(4);
    
    
    
  }
  
  void draw_game_objects(std::vector<GameObject_ref> & gos){
    bind_shader(lv1_shader);
    bind_buffer_object(square_vbo,0);
    
    for(int i = 0; i < gos.size();i++){
      GameObject_ref go = gos[i];
      Color col = go->col;
	
      lv1_shader.uniformf("size",go->sx,go->sy);
      lv1_shader.uniformf("pos",go->x,go->y);
      lv1_shader.uniformf("color",col.r,col.g,col.b,col.a);
      draw_buffers_triangle_fan(4);
        
    }
  }

  void update(){
    t = get_time() - t_start;
    
    if(t > next){
      next += 2.5;
      mouse_position mpos = get_mouse_position();
      next_go->ai = zoomin_func(next_go);
      wld->add_object(next_go);
      next_go = GameObject(randf()*2.0 - 1.0, randf()*2.0 - 1.0 ,randf()*0.2+ 0.05,randf()*0.2+ 0.05,{randf(),randf(),randf(),1.0});
      
    }
  }  

  void on_clicked_object(GameObject_ref go){
    test_proc_snd();
    as = test_proc_snd();
    play_audio_sample(&as,0);
    score += 1;
    go->ai = gof2;
  }
  
};


class TheClickening: public EventListener<MouseClick>{
public:
  Level * lv;
  TheClickening(World * _wld,Level * _lv){
    wld = _wld;
    lv = _lv;
  }
  World * wld;
  bool handle_event(MouseClick mc){
    if(mc.button == 0 && mc.pressed){
      mouse_position mpos = get_mouse_position();
      float sx = (float)(mpos.x - 256)/256;
      float sy = (float)(mpos.y - 256)/256;
      std::list<GameObject_ref> gos = wld->get_game_objects_at(sx,sy);
      if(gos.size() > 0){
	gos.reverse();
	lv->on_clicked_object(*(gos.begin()));
      }
    }
    return true;
  }

};

int test_dataflow();
void run_nodeui();


int main(){

  run_nodeui();
  return 0;
  init_ogl(512,512);
  init_audio(44100, 16, 1);
  test_proc_snd();
  
  FTGLPixmapFont font("/usr/share/fonts/truetype/msttcorefonts/Arial.ttf");
  if( font.Error() ){
    std::cout << "Error in font creation\n";
  }
  font.FaceSize(72);
  
  square_vbo  = make_buffer<float>( {-0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, 0.5}, 2,ARRAY_BUFFER, STATIC_DRAW); 
  World wld;
  Level1 lv(&wld);
  TheClickening tc(&wld,&lv);
  mouse_click_handler.register_listener(&tc);
  set_clearcolor(0.1,0.5,0.0,1.0);
  while(true){
    double start = get_time();
    lv.draw_backdrop();
    lv.draw_game_objects(wld.game_objects);
    std::for_each(wld.game_objects.begin(), wld.game_objects.end(),[&wld](GameObject_ref self){
	self->do_ai(self,&wld);
	
      });
    
    unbind_shader();
    char buffer[20] ="";
    sprintf(buffer,"%i",(int)lv.score);
    font.Render(buffer);
    swapbuffers();
    lv.update();
    wld.update();
    
    double dt = start - get_time();
    double sleep = 1.0/60.0 - dt;
    if(sleep > 0){
      sleep_sec(sleep);
    }
    
  }
  return 0;
}
