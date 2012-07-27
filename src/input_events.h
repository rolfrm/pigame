#pragma once
#include "event_handling.h"
class KeyEvent{
public:
  int key;
  bool pressed;
  bool released;
  char charkey;
  KeyEvent(int key, bool pressed){
    this->pressed = pressed;
    this->released = !pressed;
    this->key = key;
  }
};

class CharEvent{
 public:
  char charkey;
  bool pressed;
  bool released;
  CharEvent(char key,bool action){
    pressed = action;
    released = !action;
    charkey = key;
  }
};


class MouseClick{
public:
  int button;
  bool pressed;
  bool released;
  MouseClick(int button, bool pressed){
    this->pressed = pressed;
    this->released = !pressed;
    this->button = button;
  }
};

class MouseWheelEvent{
 public:
  int pos;
  int change;
  MouseWheelEvent(int _pos, int _change){
    pos = _pos;
    change = _change;
  }

};


typedef struct mouse_position{
  int x,y;
}mouse_position;

#define CTRL 289
#define ALT 291
#define ENTER 294
#define BACKSPACE 295
#define DELETE 297
extern EventSpawner<KeyEvent> key_event_handler;
extern EventSpawner<MouseClick> mouse_click_handler;
extern EventSpawner<mouse_position> mouse_move_spawner;
extern EventSpawner<CharEvent> char_event_spawner;
extern EventSpawner<MouseWheelEvent> mouse_wheel_event_spawner;
mouse_position get_mouse_position();
