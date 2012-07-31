#pragma once

#include <vector>
#include <map>
#include <string>
#include <typeinfo>
#include <iostream>
extern int cnt;
extern std::map<std::string,int> typeconv;

typedef short audio;

enum TYPES{
  FLOAT = 0,
  INT = 1,
  AUDIO = 2
};


template<class T>
int get_type(){
  std::string tname = std::string(typeid(T).name());
  std::map<std::string,int>::iterator it = typeconv.find(tname);
  if(it == typeconv.end()){
    typeconv[std::string(tname)] = cnt;
    cnt +=1;
    return cnt - 1;
  }
  return it->second;
}

void init_dataflow();

struct _input{
  bool set;
};

template <class T>
struct input{
  bool set;
  T value;  
};


enum GROUP{
  CONTROL_GRP, //500hz
  AUDIO_GRP, //44100hz
  VIDEO_GRP //30hz
};

class Node{
public:
  std::vector<std::vector<_input *> > outputs;
  std::vector<int> output_types;
  std::vector<int> input_types;
  std::vector<_input *> inputs;
  GROUP grp;

  virtual ~Node(){

  }

  Node(GROUP _grp){
    grp = _grp;
  }

  virtual void update(){
  }

  virtual void handle_click(float internal_x, float internal_y, bool keystate){
    std::cout << "Click..\n";
  }

  virtual void handle_args(std::vector<std::string> args){

  }

  template<class T>
  void register_input(input<T> & inp){
    inputs.push_back((_input *) &inp);
    input_types.push_back(get_type<T>());
  }

  template<class T>
  void register_output(){
    outputs.push_back(std::vector<_input *>());
    output_types.push_back(get_type<T>());
  }

  template<class T>
  void push_output(int index, T data){
    std::vector<_input *> &out = outputs[index];
    int l = out.size();
    for(std::vector<_input *>::iterator it = out.begin(); it != out.end() ;it++){
      input<T> * inp = (input<T> *) *it;
      inp->value = data;
      inp->set = true;
    }
  }

};
