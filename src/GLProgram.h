#pragma once
#include "gl_inc.h"
struct GLProgram{
	GLuint gl_ref,vertex_shader,fragment_shader,* ref_cnt;
};


GLProgram make_program(const char * vshader_path, const char * fshader_path);
