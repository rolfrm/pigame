#include <GL/glew.h>

#include <GL/gl.h>
#include <GL/glfw.h>

void init_ogl()
{
  //	glfwInit();
	
	glfwInit();
	glfwOpenWindow(800,600,8,8,8,8,8,8,GLFW_WINDOW);
	
		glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glewInit();


}

void swapbuffers(){
	glfwSwapBuffers();
}
