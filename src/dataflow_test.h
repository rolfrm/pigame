#pragma once
#include <math.h>
#include <iostream>

#include <string>
#include <typeinfo>
#include "shared_ptr.h"
#include "misc.h"
#include "audio.h"
#include "dataflow.h"
#include <functional>
#include "CollisionDetection.h"
#include <sstream>

class AddNode: public Node{
public:
  input<float> f1;
  input<float> f2;
  AddNode();
  void update();

};

class FloatNode:public Node{
public:
  
  input<float> f;
  FloatNode();
  void update();

  
};

class EchoFloatNode: public Node{
public:
  input<float> f;
  EchoFloatNode();
  void update();

};

class Osc: public Node{
public:
  float freq;
  float phase;
  input<float> target_freq;
  Osc(int _freq);
  void update();
};

class Phasor: public Node{
public:
  float freq;
  float phase;
  input<float> target_freq;
  Phasor(int _freq);
  void update();
};


class AddAudio:public Node{
public:
  input<audio> i1, i2;

  AddAudio();
  void update();
};


class SubAudio:public Node{
 public:
  input<audio> i1, i2;
  SubAudio();
  void update();
};

class AudioOut: public Node{
public:
  int buffersize;
  audio * buffer;
  int cnt;
  input<audio> in;
  ~AudioOut();
  AudioOut(int _buffersize);

  void update();
};

#define tone_conv_const 1.059463094 //2^(1/12)
float mtof(float note);
class line_play: public Node{
public:
  std::vector<float> f;
  float tstart;
  float dir;
  float speed;
  line_play(float _speed, std::vector<float> data);
  void handle_args(std::vector<std::string> args);
  void update();
  float t;
};
