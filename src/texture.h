#pragma once
#include <map>
#include <string>

typedef struct Texture{
	unsigned int gl_ref;
	int * ref_cnt;
	unsigned int height,width,channels;
}Texture;

Texture make_texture(void * data, int width, int height, int colorChannels, int interp_param = 0x2600, int wrap_param = 0x2901); //GL_NEAREST = 0x2600, GL_REPEAT = 2901
Texture make_texture(const char * path, int interp_param = 0x2600, int wrap_param = 0x2901);
Texture get_texture(std::string path);

void bind_texture(Texture tex, int channel);
Texture cpy_tex(Texture tex);
void del_tex(Texture tex);
void texture_test();
void unbind_texture(int channel);

extern std::map<std::string,Texture> TextureMap;
