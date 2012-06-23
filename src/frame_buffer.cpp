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
