class KeyEvent{
public:
  int key;
  bool pressed;
  bool released;
  KeyEvent(int key, bool pressed){
    this->pressed = pressed;
    this->released = !pressed;
    this->key = key;
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

typedef struct mouse_position{
  int x,y;
}mouse_position;

extern EventSpawner<KeyEvent> key_event_handler;
extern EventSpawner<MouseClick> mouse_click_handler;
extern EventSpawner<mouse_position> mouse_move_spawner;
mouse_position get_mouse_position();
