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
#include "audio.h"
extern int global_screen_width;
extern int global_screen_height;
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


int main(){

  init_audio(44100, 16, 2);
  Music m1("ko-ko.ogg");
  //play_music(&m1);
  AudioSample boom("boom.ogg");
  
  global_screen_width = 128;
  global_screen_height = 128;

  std::list<game_object *> gos;
  std::list<physical_game_object *> pobj;
  player_object a = player_object();
  player_object d = player_object();
  d.set_aabb_data(30,15,0,5,true,false);
  a.set_aabb_data(30,15,0,5,true,false);
  a.x = 10;
  a.y = 20;
  
  ObjectHandler object_handler;
  object_handler.load_object(&a);
  object_handler.load_object(&d);
  mouse_move_spawner.register_listener(&a);
  
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
  a.tex_draw = TextureDrawable(bobj,bobj2,tex2);
  d.tex_draw = a.tex_draw;
  for(int i = 0; i < 5;i++){
    game_object * c = new game_object();
    c->x = - (rand()% 128)*4 + 128;
    c->y = - (rand()% 128)*2 + 200;
    c->tex_draw = a.tex_draw;
    object_handler.load_object(c);
  }
  FrameBuffer fb(fb_tex);
  
  glUseProgram(ptest.gl_ref);
  glBindAttribLocation(ptest.gl_ref,0,"Pos");
  glBindAttribLocation(ptest.gl_ref,1,"UV_coord");
  glLinkProgram(ptest.gl_ref);
  object_handler.current_shader = ptest;
  //glUniform2f(ptest.uform_loc("scale"),(float)1.0/global_screen_width,1.0/global_screen_height);
  ptest.uniform("scale",(float)1.0/global_screen_width, 1.0/global_screen_height);
  while(true){
    
    
    bind_framebuffer(fb);
    glClear( GL_COLOR_BUFFER_BIT );

    //** Go gameloop!
    object_handler.gameloop();
    //
    unbind_framebuffer();
    bind_buffer_object(bobj3,1);
    ptest.uniform("object_scale",global_screen_width,global_screen_height);
    
    //glUniform2f(ptest.uform_loc("object_scale"),global_screen_width,global_screen_height);
    bind_texture(fb.textures[0],0);
    //glUniform2f(glGetUniformLocation(ptest.gl_ref,"off"),0.0,0.0);
    ptest.uniform("off",0.0,0.0);
    //glUniform2f(ptest.uform_loc("off"),0.0,0.0);
    glDrawArrays(GL_TRIANGLE_FAN,0,4);
    
    swapbuffers();
  }
  
  return 0;
}
