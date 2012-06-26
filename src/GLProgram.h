#pragma once
class GLProgram{
 public:
	unsigned int gl_ref,vertex_shader,fragment_shader,* ref_cnt;
	unsigned int uform_loc(const char * name);
	void uniform(const char * name, float f);
	void uniform(const char * name, float f1,float f2);
	void uniform(const char * name, float f1, float f2, float f3);
       
};


GLProgram make_program(const char * vshader_path, const char * fshader_path);
