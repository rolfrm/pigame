#pragma once
class GLProgram{
 public:
	unsigned int gl_ref,vertex_shader,fragment_shader,* ref_cnt;
	unsigned int uform_loc(const char * name);
	void uniformf(const char * name, float f);
	void uniformf(const char * name, float f1,float f2);
	void uniformf(const char * name, float f1, float f2, float f3);
	void uniformf(const char * name, float f1, float f2, float f3, float f4);
	
	void uniform(const char * name, double f);
	void uniform(const char * name, double f1,double f2);
	void uniform(const char * name, double f1, double f2, double f3);
	void uniform(const char * name, double f1, double f2, double f3, double f4);
	
	void uniform(const char * name, int f);
	void uniform(const char * name, int f1,int f2);
	void uniform(const char * name, int f1, int f2, int f3);
	void uniform(const char * name, int f1, int f2, int f3, int f4);
        void uniform(const char * name, unsigned int f);
	void uniform(const char * name, unsigned int f1,unsigned int f2);
	void uniform(const char * name, unsigned int f1, unsigned int f2, unsigned int f3);
	void uniform(const char * name, unsigned int f1, unsigned int f2, unsigned int f3, unsigned int f4);

	void bind_attr(int loc, const char * name);
	void link();
              
};
void bind_shader(GLProgram s);
void unbind_shader();

GLProgram make_program(const char * vshader_path, const char * fshader_path);
