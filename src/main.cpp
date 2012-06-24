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
#include "GLES2/gl2.h"
#include "egl_init.h"
#endif
#include "texture.h"
#include "GLProgram.h"
#include "buffer_objects.h"
#include "frame_buffer.h"

//static OpenGLContextInfo _state, *state=&_state;

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

class game_object{
public:
  float x,y,dx,dy;
  int wx,wy;
  game_object(){
    x = 0;
    y = 0.0;
    dx = 0.15;
    dy = 0.15;
    wx = 10;
    wy = 10;
  }
  virtual game_object do_ai(game_object self){
    game_object out = self;
    
   
    return out;
  }
};

class game_object2: public game_object{
public:
  int x2;
  game_object2(){
    x = 100;
    y = 0;
    wx = 10;
    wy = 10;
    x2 = -1000;
  }
};

std::list<game_object> game_loop(std::list<game_object> objects){
  std::list<game_object> out;
  for(std::list<game_object>::iterator it = objects.begin();it != objects.end();it++){
    game_object go = (*it).do_ai(*it);
    out.push_back(go);
  }
  return out;

}



int main(){

      
  // Start OGLES
  std::list<game_object> gos;
  game_object a = game_object();
  game_object2 b = game_object2();
  game_object c = b.do_ai(b);
  gos.push_back(a);
  //gos.push_back(c);
  
  init_ogl(512,512);
  
  glClearColor(0.0f, 0.0f, 0.0f, 0.1f);
  GLProgram ptest=make_program("test.vert","test.frag");
  //tex = make_texture(data,2,2,3);
  BufferObject bobj = make_buffer_object((void *)vertex,4,2,GL_FLOAT,GL_ARRAY_BUFFER,GL_STATIC_DRAW);
  bind_buffer_object(bobj,0);
  BufferObject bobj2 = make_buffer_object((void *)uv,4,2,GL_FLOAT,GL_ARRAY_BUFFER,GL_STATIC_DRAW);
  bind_buffer_object(bobj2,1);
  BufferObject bobj3 = make_buffer_object((void *)uv2,4,2,GL_FLOAT,GL_ARRAY_BUFFER,GL_STATIC_DRAW);
  
  int asd[512*512];
  memset((void *) asd,0x0,512*512*4);
  Texture fb_tex = make_texture((void *) asd,64,64,3);
  Texture tex2 = make_texture("tree123.png",GL_NEAREST,GL_REPEAT);

  FrameBuffer fb(fb_tex);
  
  glUseProgram(ptest.gl_ref);
  glBindAttribLocation(ptest.gl_ref,0,"Pos");
  glBindAttribLocation(ptest.gl_ref,1,"UV_coord");
  glLinkProgram(ptest.gl_ref);
 
  srand(time(NULL));
  timespec ts;
  float t = 0;
  game_object player;
  while(true){
    bind_buffer_object(bobj2,1);
  
    glUniform1f(glGetUniformLocation(ptest.gl_ref,"scale"),1.0);
    bind_framebuffer(fb);
    
    bind_texture(tex2,0);
    clock_gettime(CLOCK_MONOTONIC, &ts);
    t += 0.05;
    //printf("%i / %i %i\n",clock(),ts.tv_sec,ts.tv_nsec);
    //glClear( GL_COLOR_BUFFER_BIT );
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
    for(std::list<game_object>::iterator it = gos.begin(); it != gos.end();it++){
      game_object a = *it;
      *it = a.do_ai(a);
      glUniform1f(glGetUniformLocation(ptest.gl_ref,"time"),-1.0);
      glUniform2f(glGetUniformLocation(ptest.gl_ref,"off"),a.x,a.y);
      glDrawArrays(GL_TRIANGLE_FAN,0,4);
    }
    /*
      
      collision_info res=AABB_seperating_axis_collision_detection(aabb1,aabb2);
      if(res.overlap>=0.0f)
      std::cout<<res.axis[0]<<" "<<res.axis[1]<<" "<<res.overlap<<" overlap\n";	
      
      glUniform1f(glGetUniformLocation(ptest.gl_ref,"time"),res.overlap);
      glUniform2f(glGetUniformLocation(ptest.gl_ref,"off"),pos1[0],pos1[1]);
      glDrawArrays(GL_TRIANGLE_FAN,0,4);
      glUniform2f(glGetUniformLocation(ptest.gl_ref,"off"),pos2[0],pos2[1]);
      glDrawArrays(GL_TRIANGLE_FAN,0,4);*/
    bind_buffer_object(bobj3,1);
  
   glUniform1f(glGetUniformLocation(ptest.gl_ref,"deg"),0.0);
    glUniform1f(glGetUniformLocation(ptest.gl_ref,"scale"),1);
    bind_texture(fb.textures[0],0);
    unbind_framebuffer();
    glUniform2f(glGetUniformLocation(ptest.gl_ref,"off"),0.0,0.0);
    glDrawArrays(GL_TRIANGLE_FAN,0,4);
    
    swapbuffers();
  }
  
  return 0;
}
