#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <vector>
#include <math.h>
#include <iostream>
#include "CollisionDetection.h"

#include "gl_inc.h"
#ifdef GLES
#include "GLES2/gl2.h"
#include "egl_init.h"
#endif
//#include "egl_init.h"
#include "texture.h"


#include "GLProgram.h"
#include "buffer_objects.h"


//static OpenGLContextInfo _state, *state=&_state;

void init_ogl();
void swapbuffers();

float vertex[8] = {
	1.0f,1.0f,
	-1.0f,1.0f,
	-1.0f,-1.0f,
	1.0f,-1.0f,
};

float uv[8] = {
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

      
   // Start OGLES
  init_ogl();
  
  glClearColor(0.0f, 0.0f, 0.0f, 0.1f);
  GLProgram ptest=make_program("test.vert","test.frag");
  //tex = make_texture(data,2,2,3);
  BufferObject bobj = make_buffer_object((void *)vertex,4,2,GL_FLOAT,GL_ARRAY_BUFFER,GL_STATIC_DRAW);
  bind_buffer_object(bobj,0);
  BufferObject bobj2 = make_buffer_object((void *)uv,4,2,GL_FLOAT,GL_ARRAY_BUFFER,GL_STATIC_DRAW);
  bind_buffer_object(bobj2,1);
  
  glUseProgram(ptest.gl_ref);
  glBindAttribLocation(ptest.gl_ref,0,"Pos");
  glBindAttribLocation(ptest.gl_ref,1,"UV_coord");
  glLinkProgram(ptest.gl_ref);
 
  srand(time(NULL));

  float t = 0;

  while(true){
    glClear( GL_COLOR_BUFFER_BIT );
		pos1[1]+=0.005;


		aabb1[0]=pos1[0]-0.1;
		aabb1[1]=pos1[1]-0.1;
		aabb1[2]=pos1[0]+0.1;
		aabb1[3]=pos1[1]+0.1;

		aabb2[0]=pos2[0]-0.1;
		aabb2[1]=pos2[1]-0.1;
		aabb2[2]=pos2[0]+0.1;
		aabb2[3]=pos2[1]+0.1;


		collision_info res=AABB_seperating_axis_collision_detection(aabb1,aabb2);
		if(res.overlap>=0.0f)
			std::cout<<res.axis[0]<<" "<<res.axis[1]<<" "<<res.overlap<<" overlap\n";	

		glUniform1f(glGetUniformLocation(ptest.gl_ref,"time"),res.overlap);
		glUniform2f(glGetUniformLocation(ptest.gl_ref,"off"),pos1[0],pos1[1]);
    		glDrawArrays(GL_TRIANGLE_FAN,0,4);
		glUniform2f(glGetUniformLocation(ptest.gl_ref,"off"),pos2[0],pos2[1]);
    		glDrawArrays(GL_TRIANGLE_FAN,0,4);
    swapbuffers();
  }
  
  return 0;
}
