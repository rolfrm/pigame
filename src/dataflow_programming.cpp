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
#include "dataflow_test.h"

int cnt;
std::map<std::string,int> typeconv;


AddNode::AddNode():Node(CONTROL_GRP){
  register_output<float>();
  register_input(f1);
  register_input(f2);
  
}

void AddNode::update(){
  if(f1.set){
    float f = f1.value + f2.value;
    push_output(0, f);
    f1.set = false;
  }
}

SubAudio::SubAudio():Node(CONTROL_GRP){
  register_input(i1);
  register_input(i2);
  register_output<audio>();
}

void SubAudio::update(){
  audio a = i1.value - i2.value;
  push_output(0, a);
}


FloatNode::FloatNode():Node(CONTROL_GRP){
  register_output<float>();
  register_input(f);
}

void FloatNode::update(){
  
  push_output(0, f.value);
}

EchoFloatNode::EchoFloatNode():Node(CONTROL_GRP){
  register_input(f);
}
  
void EchoFloatNode::update(){
  if(f.set){
    std::cout << "ECHO FLOAT: " << f.value << "\n";
    f.set = false;
  }
}
Osc::Osc(int _freq):Node(AUDIO_GRP){
  register_output<audio>();
  register_input(target_freq);
  freq = _freq;
  target_freq.value = freq;
  phase = 0;
}
void  Osc::update(){;
  phase = fmod(phase + (float)freq/44100.0*3.14,3.14*2.0);
  freq = freq*(phase/6.28) +target_freq.value*(3.14*2.0 - phase)/6.28; 
  float out = sin(phase)*10000.0;
  
  push_output(0,(audio) out);
  
}

Phasor::Phasor(int _freq):Node(AUDIO_GRP){
  register_output<audio>();
  register_input(target_freq);
  freq = _freq;
  target_freq.value = freq;
  phase = 0;
}
void  Phasor::update(){;
  phase = fmod(phase + (float)freq/44100.0*3.14,3.14*2.0);
  freq = freq*(phase/6.28) +target_freq.value*(3.14*2.0 - phase)/6.28; 
  float out = fabs(sin(phase))*10000.0;
  
  push_output(0,(audio) out);
  
}
AddAudio::AddAudio():Node(AUDIO_GRP){
  register_output<audio>();
  register_input(i1);

  register_input(i2);
  
}
void AddAudio::update(){
  audio o = i1.value + i2.value;
  push_output(0, o);
}

AudioOut::AudioOut(int _buffersize):Node(AUDIO_GRP){
  cnt = 0;
  buffersize = _buffersize;
  buffer = new audio[_buffersize];
  register_input(in);
  in.value = 0.0;
  in.set = false;
}

void AudioOut::update(){
  buffer[cnt] = in.value;
  cnt = (cnt + 1) % buffersize;
}

AudioOut::~AudioOut(){
  std::cout << "Deleting buffer..\n";
  delete [] buffer;
}

#define tone_conv_const 1.059463094 //2^(1/12)
float mtof(float note){
  return 440*pow(tone_conv_const,note);
}

line_play::line_play(float _speed, std::vector<float> data):Node(CONTROL_GRP){
    tstart = 0;
    dir = -1.0;
    t = get_time();
    f = data;
    speed = _speed;
    register_output<float>();
  }

void line_play::handle_args(std::vector<std::string> args){
  std::vector<float> nodes;
  std::cout << "Handling args \n";
  
  for(int i = 0; i < args.size();i++){
    float arg = lexical_cast<float, std::string>(args[i]);
    if(i == 0){
      speed = arg;
    }else{
      nodes.push_back(arg);
    }
    std::cout << arg << "\n";
  }
  f = nodes;
}
void line_play::update(){
  if(f.size() > 0){  
    tstart += speed;
    
    push_output(0,mtof(f[mod((int) (tstart),f.size())]));
  }
}

void connect(Node &out, int out_index, Node &in, int in_index){
  if(out_index < 0 || out_index > out.output_types.size() || in_index < 0 || in_index > in.input_types.size()){
    return;
  }

  if(out.output_types[out_index] == in.input_types[in_index]){
    out.outputs[out_index].push_back(in.inputs[in_index]);
  }
}
