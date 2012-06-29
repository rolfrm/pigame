#include "frame_buffer.h"
#include "gl_inc.h"
#include <stdio.h>
FrameBuffer::FrameBuffer(Texture tex){
  glGenFramebuffers(1,&gl_ref);
  textures.push_back(tex);
  bind_framebuffer(*this);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,tex.gl_ref,0);
  GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
  if(status != GL_FRAMEBUFFER_COMPLETE){
    printf("Error making framebuffer\n");
  }
  textures.push_back(cpy_tex(tex));
  unbind_framebuffer();
}

FrameBuffer::FrameBuffer(std::vector<Texture> texs){
  glGenFramebuffers(1,&gl_ref);
  for(std::vector<Texture>::iterator it = texs.begin(); it != texs.end();it++){
    textures.push_back(cpy_tex(*it));
  }
  
}

FrameBuffer::FrameBuffer(int width,int height,int colorChannels, int interp_param, int wrap_param){
	textures.push_back(make_texture(0,width,height,colorChannels,interp_param, wrap_param));
	glGenFramebuffers(1,&gl_ref);
	bind_framebuffer(*this);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,textures[0].gl_ref,0);
	
	glGenRenderbuffers(1,&stencil_buffer);
	
	glBindRenderbuffer(GL_RENDERBUFFER, stencil_buffer);
	glRenderbufferStorage(GL_RENDERBUFFER,GL_STENCIL_INDEX, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_STENCIL_ATTACHMENT,GL_RENDERBUFFER, stencil_buffer);
	 
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
 	if(status != GL_FRAMEBUFFER_COMPLETE){
   		printf("Error making framebuffer\n");
 	}
}


void bind_framebuffer(FrameBuffer fb){
  glViewport(0,0,fb.textures[0].width,fb.textures[0].height);
  glBindFramebuffer(GL_FRAMEBUFFER,fb.gl_ref);
}

extern unsigned int global_width;
extern unsigned int global_height;
void unbind_framebuffer(){
  glViewport(0,0,global_width,global_height);
  glBindFramebuffer(GL_FRAMEBUFFER,0);
}

void clear_bound_framebuffer(){
  glClear(GL_COLOR_BUFFER_BIT);
}
