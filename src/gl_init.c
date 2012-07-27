#include <GL/glew.h>

#include <GL/gl.h>
#include <GL/glfw.h>
#include <IL/il.h>
#include "event_handling.h"
#include "input_events.h"
#include <list>



EventSpawner<KeyEvent> key_event_handler;
EventSpawner<MouseClick> mouse_click_handler;
EventSpawner<mouse_position> mouse_move_spawner;
EventSpawner<CharEvent> char_event_spawner;
EventSpawner<MouseWheelEvent> mouse_wheel_event_spawner;

int window_width = -1;
int window_height = -1;
void GLFWCALL mousebuttoncallback(int button, int action){
  MouseClick mEv(button,action);
  mouse_click_handler.spawn_event(mEv);

}

int last_wheel_pos = 0;
void GLFWCALL mouse_wheel_callback(int mwpos){
  MouseWheelEvent mEv(mwpos, mwpos - last_wheel_pos);
  last_wheel_pos = mwpos;
  mouse_wheel_event_spawner.spawn_event(mEv);

}


void GLFWCALL keycallback( int key, int action ){
  KeyEvent newKeyEv(key,action);
  key_event_handler.spawn_event(newKeyEv);
}
#include<iostream>
void GLFWCALL charcallback( int character, int action ){
  std::cout << character << " " << action << "\n";
  CharEvent newKeyEv(character,action==1);
  char_event_spawner.spawn_event(newKeyEv);
}

mouse_position global_mpos;

void GLFWCALL mouse_move_callback(int x, int y){
  global_mpos.x = x;
  global_mpos.y = y;
  mouse_move_spawner.spawn_event(global_mpos);
}


void init_ogl(int width, int height)
{
  window_width = width;
  window_height = height;
  ilInit();
  glfwInit();
  glfwOpenWindow(width,height,8,8,8,8,8,8,GLFW_WINDOW);
  last_wheel_pos = glfwGetMouseWheel();
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  glewInit();
  glfwSetKeyCallback(keycallback);
  glfwSetMouseButtonCallback(mousebuttoncallback);
  glfwSetMousePosCallback(mouse_move_callback);
  glfwSetCharCallback(charcallback);
  glfwSetMouseWheelCallback(mouse_wheel_callback);

}

void swapbuffers(){
  glFlush();
  glfwSwapBuffers();
}


void update_input_events(){

}


mouse_position get_mouse_position(){
  return global_mpos;
}
