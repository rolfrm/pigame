
#include "gl_inc.h"
#include<stdlib.h>
#include "texture.h"
#include <IL/il.h>
#include <iostream>

unsigned int color_channels_lookup[5] = {0, GL_LUMINANCE, GL_LUMINANCE_ALPHA, GL_RGB, GL_RGBA};

Texture make_texture(void * data, int width, int height, int colorChannels, int interp_param, int wrap_param){
  Texture out;
  out.width = width;
  out.height = height;
  out.ref_cnt = (int *) malloc(sizeof(int));
  *(out.ref_cnt) = 1;
  glGenTextures(1,&(out.gl_ref));
  glBindTexture(GL_TEXTURE_2D,out.gl_ref);
  
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, interp_param);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, interp_param);//)interp_param);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_param);//wrap_param);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_param);//wrap_param);
    
  glTexImage2D(GL_TEXTURE_2D, 0, color_channels_lookup[colorChannels],width,height,0,color_channels_lookup[colorChannels],GL_UNSIGNED_BYTE,data);
  return out;
}

Texture make_texture(const char * _path, int interp_param, int wrap_param){
  std::string path(_path);
  ILuint ilid;
  ilGenImages(1,&ilid);
  ilBindImage(ilid);

  if(ilLoadImage(path.c_str())){
    std::cout<<"texture "<<path<<" loaded\n";
    ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
    
  }else{
    std::cout << "Problems converting image" <<path<<"\n";
  }
	
  std::cout<<ilGetInteger(IL_IMAGE_WIDTH)<<" "<< ilGetInteger(IL_IMAGE_HEIGHT)
<<" dimensions\n";	
  Texture temp = make_texture((void *)ilGetData(),ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT),4,interp_param,wrap_param);
  int error = glGetError();
  if(error != 0){
    std::cout << "Error loading texture: " << error << "\n";
  }
  ilDeleteImage(ilid);
 
  return temp;
	
}

void bind_texture(Texture tex,int channel){
  glActiveTexture(GL_TEXTURE0 + channel);
  glBindTexture(GL_TEXTURE_2D, tex.gl_ref);
}

void unbind_texture(int channel){
  glActiveTexture(GL_TEXTURE0 + channel);
  glBindTexture(GL_TEXTURE_2D,0);
}

Texture cpy_tex(Texture tex){
	Texture out = tex;
	*(tex.ref_cnt) +=1;
	return out;
}
void del_tex(Texture tex){
  *(tex.ref_cnt) -=1;
  if(*(tex.ref_cnt) <= 0){
    glDeleteTextures(1,&tex.gl_ref);
    free(tex.ref_cnt);
  }
}

void texture_test(){
	unsigned char data[8] = {1,2,3,4,5,6,7,8};
	Texture tex = make_texture(data,4,2,1);	
	Texture tex2 = cpy_tex(tex2);
	del_tex(tex2);
	del_tex(tex);
}	

Texture get_texture(std::string path){
  std::map<std::string,Texture>::iterator it=TextureMap.find(path);
  if(it==TextureMap.end()){
    Texture temptex=make_texture(path.c_str());
    TextureMap.insert(std::pair<std::string,Texture>(path,temptex));
    it=TextureMap.find(path);
  }

  return it->second;
	
}

std::map<std::string,Texture> TextureMap;

