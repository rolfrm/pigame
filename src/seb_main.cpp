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

int main(){
  global_screen_width = 128;
  global_screen_height = 128;

  init_audio(44100, 16, 2);
  init_ogl(512,512);
  

  set_clearcolor(0.0f, 1.0f, 0.0f, 1.0f);


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
  
  Texture dormir = make_texture("DormusSheet.png");


  
   Music m1("ko-ko.ogg");
  Music m2(m1);
  AudioSample boom("boom.ogg");
  play_music(m2);


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
  
    Texture fb_tex = make_texture((void *) 0,global_screen_width,global_screen_height,3);  
  FrameBuffer fb(fb_tex);
  int i = 0;
  
    while(true){
    	if(glfwGetKey(GLFW_KEY_UP))
    		dorm.set_animation("dwalk");
         if(glfwGetKey(GLFW_KEY_RIGHT))
    		dorm.set_animation("walk");
        bind_framebuffer(fb);
  	  clear_bound_framebuffer();
  	  
  	/*  dorm.x=70;	
    	  dorm.y=0;
  	  
  	  dorm.draw(pshade);

    	
    	
 	dorm.x=40;
    	  dorm.y=20;
  	  
  	
  	  dorm.draw(pshade);
  	  */
  	  
  	  dorm.x=70;
    	  dorm.y=0;

	dorm.draw(psprite);
	dorm.x=40;
    	  dorm.y=20;
  	
	
    	dorm.draw(psprite);
    	
        unbind_framebuffer();
    	      bind_shader(ptest);  
    bind_buffer_object(bobj3,1);
    bind_buffer_object(bobj,0);
    	    ptest.uniformf("object_scale",global_screen_width,global_screen_height);
    set_camera_position(0,0);
      setup_shader_uniforms(ptest);
    ptest.uniformf("off",0.0,0.0);
    bind_texture(fb.textures[0],0);

    draw_buffers_triangle_fan(4);
    
    swapbuffers();
     }
	
	return 0;
}
