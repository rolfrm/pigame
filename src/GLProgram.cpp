#include "GLProgram.h"
#include <fstream>
#include <iostream>
#include <sstream>

GLuint make_shader(std::string path,GLuint type);

GLProgram make_program(const char * _vshader_path,const char * _fshader_path){
  std::string vshader_path = std::string(_vshader_path);
  std::string fshader_path = std::string(_fshader_path);
  GLProgram sprogram;
	
	sprogram.vertex_shader=make_shader(vshader_path,GL_VERTEX_SHADER);
	sprogram.fragment_shader=make_shader(fshader_path,GL_FRAGMENT_SHADER);

	sprogram.gl_ref = glCreateProgram();

 	glAttachShader(sprogram.gl_ref, sprogram.vertex_shader);
 	glAttachShader(sprogram.gl_ref, sprogram.fragment_shader);

	glLinkProgram(sprogram.gl_ref);
	GLint status;
	glGetProgramiv (sprogram.gl_ref, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetProgramiv(sprogram.gl_ref, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetProgramInfoLog(sprogram.gl_ref, infoLogLength, NULL, strInfoLog);
		fprintf(stderr, "Linker failure: %s\n", strInfoLog);
		delete[] strInfoLog;
	}		

	return sprogram;
}

GLuint make_shader(std::string path,GLuint type){
	std::ifstream is (path.c_str(),std::ios::in);
	std::stringstream buffer;
	buffer<<is.rdbuf();
	std::string shaderstring=buffer.str();

	//std::cout<<"got shader string\n"<<shaderstring<<"\n";


	GLuint shader = glCreateShader(type);
	const char * s =shaderstring.c_str();
	glShaderSource(shader,1,&s,NULL);
	glCompileShader(shader);


	GLint status=GL_FALSE;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
	        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

	        GLchar *strInfoLog = new GLchar[infoLogLength + 1];
	        glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);

	        const char *strShaderType = NULL;
	        switch(type)
	        {
	        case GL_VERTEX_SHADER: strShaderType = "vertex"; break;
	        case GL_FRAGMENT_SHADER: strShaderType = "fragment"; break;
	        }
	        std::cout<<path<<"\n";
	        fprintf(stderr, "Compile failure in %s shader:\n%s\n", strShaderType, strInfoLog);
	        delete[] strInfoLog;
	   }


	return shader;

}