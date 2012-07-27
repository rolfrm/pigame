#pragma once
#include <math.h>
#include <iostream>

#include <string>
#include <typeinfo>
#include "shared_ptr.h"
#include "misc.h"
#include "audio.h"
#include <SFML/Audio.hpp>
#include "dataflow.h"
#include <functional>
#include "CollisionDetection.h"
#include <sstream>

int mod(int x, int m);
#define n_buffers 3
#define readahead 2
std::vector<std::string> split_str(std::string str);
template<typename to, typename from>to lexical_cast(from const &x) {
  std::stringstream os;
  to ret;

  os << x;
  os >> ret;

  return ret;  
}

class out_stream:public sf::SoundStream{
public:
  std::vector<sf::Int16> buffer[n_buffers];
  int cnt;
  int lcnt;
  bool load_next;
  int buffersize;
  int sampling_freq;
  out_stream(int _buffersize,int sfreq);
  void init();

  void load_buffer(short * inbuffer,int n_samples);
  
  bool OnGetData(sf::SoundStream::Chunk& data);

};


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

class AudioOut: public Node{
public:
  int buffersize;
  audio * buffer;
  int cnt;
  input<audio> in;

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
};
void connect(Node &out, int out_index, Node &in, int in_index);
typedef std::map<std::string, shared_ptr<Node> > NodeMap;
class NodeStructure{
public:
  std::map<std::string, std::function<shared_ptr<Node>(void)> > node_creators;
  NodeMap audio_nodes;
  NodeMap control_nodes;
  out_stream o_stream;
  shared_ptr<Node> ao;
  int audio_buffer_size;

  NodeStructure(int buffersize);
  void add_node_creator(std::string str, std::function<shared_ptr<Node>(void)> nc);
  Node * new_node(std::string line);
  Node * update_node(Node * nd, std::string line);


  std::string create_node(std::string line);
  std::string create_node(std::string creator, std::string name);
  std::string create_node(std::string creator, std::string name,std::vector<std::string> args);
  shared_ptr<Node> find_node(std::string name);
  void connect_nodes(std::string inp, int inp_idx, std::string outp, int outp_idx);
  void update_node(std::string name, std::string line);
  void set_audio_out(std::string str);
  void update();
};
std::vector<std::string> str_vec_from_float_vec(std::vector<float> in);
NodeStructure * gen_standard_node_structure();
