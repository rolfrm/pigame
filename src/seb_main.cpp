#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <vector>
#include <list>
#include <math.h>
#include <iostream>
#include "CollisionDetection.h"

#include "texture.h"
#include "GLProgram.h"
#include "buffer_objects.h"
#include "frame_buffer.h"

#include "event_handling.h"
#include "input_events.h"
#include "game_objects.h"
#include "game.h"
#include "audio.h"
#include "gfx_basics.h"
#include "game_super.h"

#include <GL/glfw.h>

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

extern int global_screen_width;
extern int global_screen_height;

float axis[2]={0,-1};

bool compare_sprite_axis(SpriteSheetDrawable * first,SpriteSheetDrawable * second){
	float first_projection=axis[0]*first->x+axis[1]*first->y;
	float second_projection=axis[0]*second->x+axis[1]*second->y;
	if(first_projection<second_projection)
		return true;
	else
		return false;
}


int main(){
  global_screen_width = 128;
  global_screen_height = 128;

  init_audio(44100, 16, 2);
  init_ogl(512,512);
  

 


  GLProgram ptest=make_program("test.vert","test.frag");
  ptest.bind_attr(0,"Pos");
  ptest.bind_attr(1,"UV_coord");
  ptest.link();
  bind_shader(ptest);  
  ptest.uniformf("scale",(float)1.0/global_screen_width, 1.0/global_screen_height);
  
  
  
 GLProgram psprite=make_program("sprite.vert","sprite.frag");
  psprite.bind_attr(0,"Pos");
  psprite.bind_attr(1,"UV_coord");
  psprite.link();
  bind_shader(psprite);  
  

   GLProgram pshade=make_program("shadow.vert","shadow.frag");
  pshade.bind_attr(0,"Pos");
  pshade.bind_attr(1,"UV_coord");
  pshade.link();
  
  GLProgram pshadowOnObject=make_program("shading.vert","shading.frag");
  pshadowOnObject.bind_attr(0,"Pos");
  pshadowOnObject.bind_attr(1,"UV_coord");
  pshadowOnObject.link();
  
  Texture dormir = make_texture("DormusSheet.png");


  
   Music m1("ko-ko.ogg");
  Music m2(m1);
  AudioSample boom("boom.ogg");
 // play_music(m2);


  BufferObject bobj = make_buffer_object((void *)vertex,4,2,FLOAT);
  BufferObject bobj2 = make_buffer_object((void *)uv,4,2,FLOAT);
    BufferObject bobj3 = make_buffer_object((void *)uv2,4,2,FLOAT);
  
  SpriteSheetDrawable dorm(bobj,bobj2,dormir);
  
  
  dorm.load_animation_frame("walk",20,20,20,0,0.2);
  dorm.load_animation_frame("walk",20,20,0,0,0.2);
  dorm.load_animation_frame("walk",20,20,40,0,0.2);
  dorm.load_animation_frame("walk",20,20,0,0,0.2);
  
  dorm.load_animation_frame("dwalk",20,20,20,20,0.2);
    dorm.load_animation_frame("dwalk",20,20,40,20,0.2);
  dorm.load_animation_frame("dwalk",20,20,0,20,0.2);
    dorm.load_animation_frame("dwalk",20,20,40,20,0.2);
  
  dorm.load_animation_frame("uwalk",20,20,20,40,0.2);
  dorm.load_animation_frame("uwalk",20,20,0,40,0.2);
  
  
  dorm.set_animation("walk");
    	  
  dorm.x=50;	
  dorm.y=-10;
  SpriteSheetDrawable dorm2(dorm);
  
 dorm2.x=40;
  dorm2.y=20;
  
  std::list<SpriteSheetDrawable *> sprites;
  sprites.push_back(&dorm);
  sprites.push_back(&dorm2);
  
   // Texture fb_tex = make_texture((void *) 0,global_screen_width,global_screen_height,3);  
  //FrameBuffer fb(fb_tex);
  FrameBuffer shadowbuffer(global_screen_width,global_screen_height,4);
    FrameBuffer rendertarget(global_screen_width,global_screen_height,4);
  
    while(true){
    	if(glfwGetKey(GLFW_KEY_UP))
    		dorm.y-=1;
         if(glfwGetKey(GLFW_KEY_RIGHT))
    		dorm.x-=1;
    		
    	 if(glfwGetKey(GLFW_KEY_DOWN))
    		dorm.y+=1;
         if(glfwGetKey(GLFW_KEY_LEFT))
    		dorm.x+=1;
        bind_framebuffer(rendertarget);
         set_clearcolor(0.0f, 0.0f, 0.0f, 0.0f);
          clear_bound_framebuffer();
        
        
        glClear(GL_STENCIL_BUFFER_BIT);
        glEnable(GL_STENCIL_TEST);
        glStencilOp(GL_KEEP,GL_KEEP,GL_REPLACE);
  	
  	
  	axis[0]=0;
  	axis[1]=-1;
  	sprites.sort(compare_sprite_axis);
  	int i=0;
	for(std::list<SpriteSheetDrawable *>::iterator it=sprites.begin();it!=sprites.end();it++){
		(*it)->ID=sprites.size()-i;
		glStencilFunc(GL_GREATER,(*it)->ID,0xFF);
		(*it)->draw(psprite);
		i++;
	}
  	
	
//    	dorm2.draw(psprite);
    	
    	
    	 bind_framebuffer(shadowbuffer);
    	  set_clearcolor(1.0f, 1.0f, 1.0f, 1.0f);
         clear_bound_framebuffer();
  	
  	axis[0]=-0.5;
  	axis[1]=-1.5;
  	sprites.sort(compare_sprite_axis); 	
    	pshade.uniformf("lDirection",axis[0],axis[1]);
    	pshadowOnObject.uniformf("lDirection",axis[0],axis[1]);
    	pshadowOnObject.uniform("shadowTexture",1);
    	bind_texture(shadowbuffer.textures[0],1);
    	
    	for(std::list<SpriteSheetDrawable *>::iterator it=sprites.begin();it!=sprites.end();it++){
    		bind_framebuffer(rendertarget);
    		glStencilFunc(GL_EQUAL,(*it)->ID,0xFF);
    		
    		(*it)->draw(pshadowOnObject);
    		
    		bind_framebuffer(shadowbuffer);
    		glStencilFunc(GL_ALWAYS,1,0xFF);
    		(*it)->draw(pshade);
    	
    	}

    	
    	//bind_shader(pshadowOnObject);
    	//draw_buffers_triangle_fan(4);
    	
    	 glDisable(GL_STENCIL_TEST);
    	 
    	 
    	 
    	 
    	 
    	 
    	 
        unbind_framebuffer();
         set_clearcolor(0.0f, 1.0f, 0.0f, 1.0f);
        clear_bound_framebuffer();
    	      bind_shader(ptest);  
    bind_buffer_object(bobj3,1);
    bind_buffer_object(bobj,0);
    	    ptest.uniformf("object_scale",global_screen_width,global_screen_height);
    set_camera_position(0,0);
      setup_shader_uniforms(ptest);
    ptest.uniformf("off",0.0,0.0);
  /*  if(glfwGetKey(GLFW_KEY_LEFT)){
   	 bind_texture(shadowbuffer.textures[0],0);
    }
    else{
	    bind_texture(rendertarget.textures[0],0);
	}
	*/
	 bind_texture(shadowbuffer.textures[0],0);
    draw_buffers_triangle_fan(4);
    	 bind_texture(rendertarget.textures[0],0);
    draw_buffers_triangle_fan(4);
    
    swapbuffers();
     }
	
	return 0;
}
