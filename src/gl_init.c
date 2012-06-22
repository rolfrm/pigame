#include <GL/glew.h>

#include <GL/gl.h>
#include <GL/glfw.h>
unsigned int global_width, global_height;
void init_ogl(int width, int height)
{

  global_width = width;
  global_height = height;
	
  glfwInit();
  glfwOpenWindow(global_width,global_height,8,8,8,8,8,8,GLFW_WINDOW);
	
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  glewInit();


}

void swapbuffers(){
	glfwSwapBuffers();
}
