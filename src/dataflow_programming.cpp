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
#include "dataflow_test.h"
#include <sstream>
int mod(int x, int m) {
    return (x%m + m)%m;
}
int cnt;
std::map<std::string,int> typeconv;

std::vector<std::string> split_str(std::string str){
  std::vector<std::string> out;
  while(true){
    if(str.size() == 0){
      return out;
    }
    int pos = str.find(' ');
    std::cout << pos << " " << str << " " << pos << str[pos] << "\n";
    if(str[pos] == ' '){
      std::string st = str.substr(0,pos);
      if(st != ""){
	out.push_back(st);
      }
      str = str.substr(pos+1,str.size());
    }else{
      if(str != ""){
	out.push_back(str);
      }
      return out;
    }
  }
  return out; 
}

void split_test(){
  std::vector<std::string> st = split_str("   asd  dsa   w  d    a  sdsa   s ");
  for(int i = 0; i< st.size();i++){
    std::cout << "|" << st[i]<< "|" << "\n";
  }
}






out_stream::out_stream(int _buffersize,int sfreq){
  buffersize = _buffersize;
  sampling_freq = sfreq;
}


void out_stream::init(){
  
  Initialize(1,sampling_freq);
  cnt = 0;
  lcnt = 0;
  for(int i = 0; i < n_buffers;i++){
    buffer[i] = std::vector<sf::Int16>(buffersize);
  }
  load_next = false;
}

void out_stream::load_buffer(short * inbuffer,int n_samples){
  //std::cout <<"load:" <<  cnt << " " << lcnt << "\n";
  int lbuf = (cnt + 1)%n_buffers;
  buffer[lbuf] = std::vector<sf::Int16>(inbuffer, inbuffer + n_samples);
  lcnt = (lcnt+ 1)%n_buffers;
  if( mod( (lcnt - readahead)  ,n_buffers) ==cnt){
    load_next = false;
  }
}
  
bool out_stream::OnGetData(sf::SoundStream::Chunk& data){
  //std::cout <<"read:" <<  cnt << " " << lcnt << "\n";
    
  data.Samples = &buffer[cnt][0];
  data.NbSamples = buffer[cnt].size();
  cnt = (cnt + 1)%n_buffers;
  load_next = true;
  return true;
}

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

#define tone_conv_const 1.059463094 //2^(1/12)
float mtof(float note){
  return 440*pow(tone_conv_const,note);
}

line_play::line_play(float _speed, std::vector<float> data):Node(CONTROL_GRP){
    tstart = 0.0;
    dir = -1.0;
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
    //postd::cout << mtof(f[mod((int) (tstart),f.size())]) << "\n";
    tstart += speed;
    //std::cout << mtof(f[mod((int) (tstart),f.size())]) << "lp\n";
    
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

void NodeStructure::add_node_creator(std::string str, std::function<shared_ptr<Node>(void)> nc){
  node_creators[str] = nc;
  
}
int str_cnt = 0;
std::string NodeStructure::create_node(std::string creator, std::string name){
  std::vector<std::string> args;
  return create_node(creator,name,args);
}



Node * NodeStructure::new_node(std::string line){
  if(line == ""){
    return NULL;
  }
  std::vector<std::string> args;
  std::vector<std::string> line_split = split_str(line);
  for(int i = 1; i < line_split.size();i++){
    args.push_back(line_split[i]);
  }
  std::string name = line_split[0];
  

  if(node_creators.find(name) != node_creators.end()){
    
    shared_ptr<Node> nd = node_creators.find(name)->second();
    nd->handle_args(args);
    *(nd.ref) += 10;
    return nd.data;

  }
  return NULL;
}

Node * NodeStructure::update_node(Node *nd, std::string line){
  delete nd;
  return new_node(line);
}

std::string NodeStructure::create_node(std::string creator, std::string name, std::vector<std::string> args){

  if(name == ""){
    name = lexical_cast<std::string, int>(str_cnt);
    str_cnt++;
  }

  shared_ptr<Node> new_node = node_creators.find(creator)->second();
  new_node->handle_args(args);
  std::map<std::string, shared_ptr<Node> > * mp = new_node->grp == AUDIO_GRP ? &audio_nodes : (new_node->grp == CONTROL_GRP ? &control_nodes : NULL);
  if(mp == NULL){
    throw "Error: unknown group";
  }  
  (*mp)[name] = new_node;
  return name;
  
}

shared_ptr<Node> NodeStructure::find_node(std::string name){
  NodeMap::iterator nodemap_it =audio_nodes.find(name);
  if(nodemap_it != audio_nodes.end()){
    return nodemap_it->second;
  }
  nodemap_it = control_nodes.find(name);
  if(nodemap_it != control_nodes.end()){
    return nodemap_it->second;
  }
  throw "Node not found";
}


void NodeStructure::connect_nodes(std::string inp, int inp_idx, std::string outp, int outp_idx){
  Node * in = find_node(inp).data;
  Node * out = find_node(outp).data;

  connect(*in,inp_idx,*out,outp_idx);
}
NodeStructure::NodeStructure(int buffersize):o_stream(buffersize,44100){
  audio_buffer_size = buffersize;
  o_stream.init();
  o_stream.Play();
  
}

void NodeStructure::set_audio_out(std::string str){
  ao = audio_nodes.find(str)->second;
}


void NodeStructure::update(){
  AudioOut * aoo = (AudioOut *) ao.data;
    

  for(int i = 0; i < audio_buffer_size;i++){
    if( mod(i,64) == 0){
      for(std::map<std::string,shared_ptr<Node>>::iterator it = control_nodes.begin(); it != control_nodes.end();it++){
	(*it).second->update();
      }
    }
    for(std::map<std::string,shared_ptr<Node>>::iterator it= audio_nodes.begin();  it != audio_nodes.end();it++){
	(*it).second->update();
	
    }
  }
  if(ao.data != NULL){
  AudioSample tempsamp((char *) aoo->buffer, aoo->buffersize, 2, 1);
  o_stream.load_buffer(aoo->buffer,audio_buffer_size);
  while(o_stream.load_next == false){
    sleep_sec(5.0/44100.0);
  }
  }
}


#define ADD_NS(ns, name, cst) ns->add_node_creator( name, [](){ return shared_ptr<Node>(cst);})

NodeStructure * gen_standard_node_structure(){
  NodeStructure * ns = new NodeStructure(8*1024);
  ADD_NS(ns,"osc",new Osc(440));
  ADD_NS(ns,"audio-out", new AudioOut(8*1024));
  ADD_NS(ns,"add-audio", new AddAudio());
  ADD_NS(ns,"line-play", new line_play(0.01,std::vector<float>()));
  return ns;
}

#undef ADD_NS
std::vector<std::string> str_vec_from_float_vec(std::vector<float> in){
  std::vector<std::string> out;
  for(int i = 0; i < in.size();i++){
    out.push_back(lexical_cast<std::string, float>(in[i]));
  }
  return out;
}

int test_dataflow(){
  std::vector<float> line1 = std::vector<float>({0.01, 0.0,0 - 12.0,0.0,0 - 12.0,5.0,5.0 - 12.0,5.0,5.0-12.0,3.0,3.0-12.0,3.0,3.0-12.0 ,5.0,5.0-12.0,7.0, 7.0 - 12.0});
  std::vector<float> line2 = std::vector<float>({0.01/16.0, 12.0, 10, 8.0,7.0});

  NodeStructure & ns = *gen_standard_node_structure();
  
  ns.create_node("line-play","lp1",str_vec_from_float_vec(line1));
  ns.create_node("line-play","lp2",str_vec_from_float_vec(line2));
  ns.create_node("osc", "osc1");
  ns.create_node("osc", "osc2");
  ns.create_node("add-audio", "aao");
  ns.create_node("audio-out", "ao1");
  ns.connect_nodes("lp1",0,"osc1",0);
  ns.connect_nodes("lp2",0,"osc2",0);
  ns.connect_nodes("osc1", 0, "aao",0); 
  ns.connect_nodes("osc2", 0, "aao",1);
  ns.connect_nodes("aao",0, "ao1",0);
  ns.set_audio_out("ao1");

  for(int i = 0; i < 100;i++){
    ns.update();
  }
  
  return 0;
}
