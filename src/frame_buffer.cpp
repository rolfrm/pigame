#include "frame_buffer.h"
#include "gl_inc.h"
FrameBuffer::FrameBuffer(Texture tex){
  glGenFramebuffers(1,&gl_ref);
  textures.push_back(tex);
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
void unbind_framebuffers(){
  glViewport(0,0,global_width,global_height);
}
