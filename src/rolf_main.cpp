#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <vector>
#include <list>
#include <math.h>
#include <iostream>
#include "CollisionDetection.h"

#include "gl_inc.h"
#ifdef GLES
#include "egl_init.h"
#endif
#include "texture.h"
#include "GLProgram.h"
#include "buffer_objects.h"
#include "frame_buffer.h"

//static OpenGLContextInfo _state, *state=&_state;
#include "event_handling.h"
#include "input_events.h"
#include "game_objects.h"
#include "game.h"
void init_ogl(int width, int height);
void swapbuffers();

float vertex[8] = {
	-1.0f,1.0f,
	1.0f,1.0f,
	1.0f,-1.0f,
	-1.0f,-1.0f,
};

float uv[8] = {
	1.0f,1.0f,
	0.0f,1.0f,
	0.0f,0.0f,
	1.0f,0.0f,
};

float uv2[8] = {
	1.0f,0.0f,
	0.0f,0.0f,
	0.0f,1.0f,
	1.0f,1.0f,
};

float aabb1[4];
float aabb2[4];
float pos1[2]={-0.15f,-1.0f};
float pos2[2]={0.0f,0.0f};


void game_loop(std::list<game_object * > objects){
  for(std::list<game_object *>::iterator it = objects.begin();it != objects.end();it++){
    (*it)->do_ai();
  }

}

class TestKeyboardListener: public EventListener<KeyEvent>, public EventListener<MouseClick>, public EventListener<mouse_position>{
public:
  void handle_event(KeyEvent kev){
    std::cout  << kev.key << "<- key press\n";
  }
  void handle_event(MouseClick mev){
    std::cout  << mev.button << "<- mouse click\n";
  }
  void handle_event(mouse_position mpos){
    mpos = screen_pos_to_world_pos(mpos);
    std::cout << mpos.x << " " <<mpos.y <<"<- mouse move\n";
  }
};

bool z_compare(Drawable * d1, Drawable * d2){
  return d1->z > d2->z;
}


int main(){
  global_screen_width = 128;
  global_screen_height = 128;
  TestKeyboardListener * tkel = new TestKeyboardListener();
  key_event_handler.register_listener(tkel);
  mouse_click_handler.register_listener(tkel);
  mouse_move_spawner.register_listener(tkel);

  std::list<game_object *> gos;
  player_object a = player_object();
  player_object b = player_object();
  mouse_move_spawner.register_listener(&a);
  gos.push_back(&a);
  gos.push_back(&b);
  //gos.push_back(c);
  
  init_ogl(512,512);
  
  glClearColor(0.0f, 0.0f, 0.0f, 0.1f);
  GLProgram ptest=make_program("test.vert","test.frag");
  BufferObject bobj = make_buffer_object((void *)vertex,4,2,GL_FLOAT,GL_ARRAY_BUFFER,GL_STATIC_DRAW);
  bind_buffer_object(bobj,0);
  BufferObject bobj2 = make_buffer_object((void *)uv,4,2,GL_FLOAT,GL_ARRAY_BUFFER,GL_STATIC_DRAW);
  bind_buffer_object(bobj2,1);
  BufferObject bobj3 = make_buffer_object((void *)uv2,4,2,GL_FLOAT,GL_ARRAY_BUFFER,GL_STATIC_DRAW);
  
  int asd[512*512];
  memset((void *) asd,0x0,512*512*4);
  Texture fb_tex = make_texture((void *) asd,global_screen_width,global_screen_height,3);
  Texture tex2 = make_texture("tree123.png",GL_NEAREST,GL_REPEAT);
  a.tdraw = TextureDrawable(bobj,bobj2,tex2);
  b.tdraw = a.tdraw;
  for(int i = 0; i < 5;i++){
    player_object * c = new player_object();
    c->x = - (rand()% 128)*4 + 128;
    c->y = - (rand()% 128)*2 + 200;
    c->tdraw = a.tdraw;
    gos.push_back(c);
  }
  FrameBuffer fb(fb_tex);
  
  glUseProgram(ptest.gl_ref);
  glBindAttribLocation(ptest.gl_ref,0,"Pos");
  glBindAttribLocation(ptest.gl_ref,1,"UV_coord");
  glLinkProgram(ptest.gl_ref);
 
  srand(time(NULL));
  timespec ts;
  float t = 0;
  game_object player;
  glUniform2f(glGetUniformLocation(ptest.gl_ref,"scale"),(float)1.0/global_screen_width,1.0/global_screen_height);
    
  while(true){
    bind_buffer_object(bobj2,1);
  
    
    bind_framebuffer(fb);
    
    bind_texture(tex2,0);
    clock_gettime(CLOCK_MONOTONIC, &ts);
    t += 0.05;
    //printf("%i / %i %i\n",clock(),ts.tv_sec,ts.tv_nsec);
    glClear( GL_COLOR_BUFFER_BIT );
    player.x +=sin(t)*5;
    player.y +=cos(t)*5;
    
    pos1[1]+=0.005;


    aabb1[0]=pos1[0]-0.1;
    aabb1[1]=pos1[1]-0.1;
    aabb1[2]=pos1[0]+0.1;
    aabb1[3]=pos1[1]+0.1;
    
    aabb2[0]=pos2[0]-0.1;
    aabb2[1]=pos2[1]-0.1;
    aabb2[2]=pos2[0]+0.1;
    aabb2[3]=pos2[1]+0.1;
    std::list<Drawable *> drawlist;
    for(std::list<game_object *>::iterator it = gos.begin(); it != gos.end();it++){
      game_object * a = *it;
      a->do_ai();
      drawlist.push_back(a->draw());
      
    }
    drawlist.sort(z_compare);
    for(std::list<Drawable *>::iterator it = drawlist.begin();it != drawlist.end();it++){
      (*it)->draw(ptest);
    }
    unbind_framebuffer();
    bind_buffer_object(bobj3,1);
    glUniform2f(glGetUniformLocation(ptest.gl_ref,"object_scale"),global_screen_width,global_screen_height);
    bind_texture(fb.textures[0],0);
    
    glUniform2f(glGetUniformLocation(ptest.gl_ref,"off"),0.0,0.0);
    glDrawArrays(GL_TRIANGLE_FAN,0,4);
    
    swapbuffers();
  }
  
  return 0;
}
