#include "dataflow_test.h"
#include "dataflow.h"

#include "gfx_basics.h"
#include "buffer_objects.h"
#include "GLProgram.h"


#include "misc.h"
#include "event_handling.h"
#include "input_events.h"
#include <FTGL/ftgl.h>
#include "CollisionDetection.h"
#include <math.h>

#include <iostream>
#include <algorithm>
#include "math.h"
extern int window_width;
extern int window_height;
extern float camera_x;
extern float camera_y;
float scale = 0.25;
#define FONT "/var/lib/defoma/x-ttcidfont-conf.d/dirs/TrueType/Courier_New.ttf"

class ConnectionInfo{
public:
  Node * out_node;
  Node * in_node;
  int inlet;
  int outlet;
  bool is_sane(){
    
    if(out_node->output_types.size() <=outlet || in_node->input_types.size() <= inlet || inlet < 0 || outlet < 0 ){
      return false;
    }
    
    if(out_node->output_types[outlet] == in_node->input_types[inlet]){
      return true;
    }
    return false;
  }
};


class NodeBackend{
public:
  std::list<ConnectionInfo> connections;
  std::list<Node *> nodes;
  void insert_node(Node * nd){
    nodes.push_back(nd);
  }

  void remove_node(Node *nd){
    nodes.remove(nd);
    std::list<ConnectionInfo> new_connections;
    for(auto it = connections.begin();it != connections.end();it++){
      if(it->out_node == nd){
	continue;
      }
      if(it->in_node == nd){
	continue;
      }
      new_connections.push_back(*it);
      }

    connections = new_connections;

  }

  void swap_node(Node * old, Node * _new){
    nodes.remove(old);
    nodes.push_back(_new);
    std::list<ConnectionInfo> new_connections;
    for(auto it = connections.begin();it != connections.end();it++){
      if(it->out_node == old){
	it->out_node = _new;
	
	std::cout << "1 This happened.. \n";
	if(it->outlet >= 0 && it->outlet < _new->outputs.size()){
	  std::cout << "and this.. \n";
	  _new->outputs[it->outlet].push_back(it->in_node->inputs[it->inlet]);
	}
      }
      if(it->in_node == old){
	std::cout << "2 This happened.. \n";
	std::vector<_input *> &vec = it->out_node->outputs[it->outlet];
	_input * removethis = it->in_node->inputs[it->inlet];
	for(auto it2 = vec.begin(); it2 != vec.end();it2++){
	  if(removethis == *it2){
	    if(it->inlet < _new->inputs.size()){
	      *it2 = _new->inputs[it->inlet];
	    }else{
	      vec.erase(it2);
	    }
	    break;
	  }
	}

	it->in_node = _new;
      }
      if(it->is_sane()){
	new_connections.push_back(*it);
      }
    }

    connections = new_connections;

  }

  Node * find_node(Node *nd){
    for(auto it = nodes.begin();it != nodes.end();it++){
      if(*it == nd){
	return nd;
      }
    }
    return NULL;
  }
  void connect_nodes(Node * out, Node * in, int outlet, int inlet){
    if(find_node(out) == NULL || find_node(in) == NULL){
      std::cout << "Error: Unknown nodes\n";
      return;
    }

    if(out->output_types.size() <= outlet || outlet < 0 || inlet >= in->input_types.size() || inlet < 0 ){
      std::cout << "Error: Bad outlet or inlet\n";
      return;
    }
    if(out->output_types[outlet] != in->input_types[inlet]){
      std::cout << "Error: Bad type \n";
      return;
    }

      out->outputs[outlet].push_back(in->inputs[inlet]);

      connections.push_back(ConnectionInfo({out,in,inlet,outlet}));
    
  }

    void update(){
      for(auto it = nodes.begin();it != nodes.end();it++){
	(*it)->update();
      }
    }
};




float line_point_distance(float lx1, float ly1, float lx2, float ly2, float px, float py){
  lx1 -= lx2;
  px  -= lx2;
  ly1 -= ly2;
  py  -= ly2;

  float lv1 = sqrt(lx1*lx1 + ly1*ly1);
  float lp = sqrt(px*px + py*py);
  float cosa = (px*lx1 + py*ly1) / (lp*lv1); //dotproduct divided by mult length  = cos angle
  float a = lp*cosa;
  
  float pard = 0; //Distance parallel to line
  if(a < 0){
    pard = -a;
  }else if(a > lv1){
    pard = a - lv1;
  }
  float b = a/lv1;

  float ppx = b*lx1;
  float ppy = b*ly1;

  float dx = px - ppx;
  float dy = py - ppy;
  float ortho_d = sqrt(dx*dx + dy*dy); //Distance orthogonal to line

  float d = sqrt(ortho_d*ortho_d + pard*pard);
  return d;
}


#undef FLOAT
struct Color{
  float r,g,b,a;
};
extern BufferObject square_vbo;
BufferObject line_vbo;

class UiBox{
public:
  Vec2f pos; //upper left corner
  Vec2f size;
  
  Color bg,border;
  float width;

  UiBox(Vec2f _pos, Vec2f _size,Color bg_color,float _width, Color _border ):pos(_pos),size(_size){
    bg = bg_color;
    border = _border;
    width = _width;
  }

  bool check_point(Vec2f point){
    point = point - pos;
    return !(point.x < 0 || point.x > size.x || point.y < 0 || point.y > size.y);
  }

  void draw(GLProgram &s){
    bind_buffer_object(square_vbo,0);
    s.uniformf("pos",pos.x,pos.y);
    s.uniformf("size",size.x,size.y);
    s.uniformf("color",bg.r,bg.g,bg.b,bg.a);
    
    draw_buffers_triangle_fan(4);
    if(border.a != 0.0 && width > 0.0){
      set_line_width(ceil(width*scale));
      s.uniformf("color",border.r,border.g,border.b,border.a);
      draw_buffers_line_loop(4);
    }
  }
};

class UiLine{
public:
  Vec2f pos_start, pos_end;
  Color line_color;
  float width;
  UiLine(Vec2f _pos_start, Vec2f _pos_end, Color _line_color):pos_start(_pos_start),pos_end(_pos_end),line_color(_line_color){
    width = 1.0;
  }

  void draw(GLProgram &s){
    set_line_width(ceil(width*scale));
    bind_buffer_object(line_vbo,0);
    s.uniformf("pos",pos_start.x,pos_start.y); //Assume line attr = {(0,0),(1,1)}
    s.uniformf("size", pos_end.x - pos_start.x , pos_end.y - pos_start.y);
    s.uniformf("color",line_color.r,line_color.g,line_color.b,line_color.a);
    draw_buffers_line_loop(2);
  }

  bool check_point(Vec2f p, float distance_threshold){
    float d = line_point_distance(pos_start.x,pos_start.y,pos_end.x,pos_end.y,p.x,p.y);
    float scaling =scale > 1 ? sqrt(scale) : 1.0;
    return d < distance_threshold/scaling;
  }
};

class UiTextBox{
public:
  UiBox box;
  Vec2f pos;
  shared_ptr<FTGLPixmapFont> font;
  std::string text;
  float padding;
  float fontsize;
  UiTextBox(Vec2f _pos,shared_ptr<FTGLPixmapFont> _font):font(_font),box(_pos,Vec2f(0,0),{0.0,0.0,0.0,1.0},1.0,{1.0,1.0,1.0,1.0}), pos(_pos){
    padding = 30.0;
    fontsize = 32;
  }

  bool check_point(Vec2f point){
    return box.check_point(point);
  }

  void draw(GLProgram &s){
    float *a = new float[1000];
    delete [] a;
    font->FaceSize(fontsize);
    FTBBox textbox= font->BBox(text.c_str());
    
    box.pos = pos;
    float w = textbox.Upper().X() - textbox.Lower().X()+padding*2.0;
    float h = textbox.Upper().Y() - textbox.Lower().Y()+ padding*2.0;
    box.size = Vec2f(w,h);
    box.draw(s);
    unbind_shader();
    font->FaceSize(fontsize*scale);
      font->Render(text.c_str(),-1,FTPoint( ((pos.x - camera_x)  + padding)*scale + window_width/2, ((pos.y - camera_y)  + padding)*scale+ window_height/2));
    bind_shader(s);

  }
};

Vec2f mouse_to_world(mouse_position mp){
  Vec2f v(mp.x - window_width/2, mp.y - window_height/2);
  v.y *=-1.0;
  return (v*(1/scale)) + Vec2f(camera_x,camera_y);
}

Color type_colors[3] = { {0.0,0.0,1.0,1.0}, {0,1.0,0.0,1.0}, {1.0,0.0,0.0,1.0}};

#define INLET_SIZE 20 //px
class NodeViewer{
public:
  UiTextBox body;
  std::vector<UiBox> inlets;
  std::vector<UiBox> outlets;
  NodeViewer(Vec2f pos,shared_ptr<FTGLPixmapFont> fnt,std::string text = ""):body(pos,fnt){
 
    body.text = text;
  }

  

  void set_position(Vec2f p){
    body.pos = p;
  }

  void update(){
    Vec2f bpos = body.pos;
    int n_inlet = 0;
    float offset = body.box.size.y;
    auto inlet_func = [&](UiBox &ui){ ui.pos = Vec2f(bpos.x + INLET_SIZE*n_inlet++,bpos.y + offset);};
    for_each(inlets.begin(),inlets.end(), inlet_func);

    n_inlet = 0;
    offset = -INLET_SIZE;

    for_each(outlets.begin(),outlets.end(),inlet_func);
  }

  void sanitize(Node * node){
    
    inlets.clear();
    outlets.clear();
    if(node == NULL){
      return;
    }
    
    for(int i = 0; i < node->input_types.size();i++){
      int type = node->input_types[i];
      inlets.push_back(UiBox(body.pos,Vec2f(INLET_SIZE,INLET_SIZE),type_colors[type],1.0,{1.0,1.0,1.0,1.0}));
    }

    for(int i = 0; i < node->output_types.size();i++){
      int type = node->output_types[i];
      outlets.push_back(UiBox(body.pos,Vec2f(INLET_SIZE,INLET_SIZE),type_colors[type],1.0,{1.0,1.0,1.0,1.0}));
    }

  }

  void draw(GLProgram &s){
    body.draw(s);
    update();
    for(int i = 0; i < inlets.size();i++){
      inlets[i].draw(s);
    }
    for(int i = 0; i < outlets.size();i++){
      outlets[i].draw(s);
    }
  }


};

NodeViewer * find_nodeviewer_instance(std::list<NodeViewer *> &nvs,NodeViewer * nv){
  for(auto it = nvs.begin(); it != nvs.end();it++){
    if(nv == (*it)){
      return (*it);
    }
  }
  return NULL;
}

class ConnectionLine: public UiLine{
public:
  NodeViewer * start;
  int outlet;
  std::vector<_input *>::iterator outlet_nr;

  NodeViewer * end;
  int inlet;

  ConnectionLine(NodeViewer * _start, int _outlet, NodeViewer * _end, int _inlet):UiLine(Vec2f(0.0,0.0),Vec2f(0.0,0.0),{1.0,1.0,1.0,1.0}){
    start = _start;
    end = _end;
    inlet = _inlet;
    outlet = _outlet;

  }

  

  void update(std::list<NodeViewer *> &nvs){

    pos_start = start->outlets[outlet].pos + Vec2f(INLET_SIZE/2,INLET_SIZE/2);
    pos_end = end->inlets[inlet].pos + Vec2f(INLET_SIZE/2,INLET_SIZE/2);
      
  }

  bool operator==(const ConnectionLine &other){
    return start == other.start && end == other.end && outlet == other.outlet && inlet == other.inlet;
  }

};
class NodeGrid{
public:
  typedef std::map<std::string, std::function< Node * (void)>> FuncMap;
  typedef std::map<Node * , NodeViewer *> NodeView;


  std::list<NodeViewer *> node_viewers;
  FuncMap node_creators;
  NodeView node_view;
  shared_ptr<FTGLPixmapFont> font;
  GLProgram shader;
  NodeBackend backend;
  
  NodeGrid(){
    font = new FTGLPixmapFont(FONT);
  }

  Node * find_node_from_nodeview(NodeViewer * nv){
    for(auto it = node_view.begin();it != node_view.end();it++){
      if(it->second == nv){
	return it->first;
      }
    }
    return NULL;

  }
  
  Node * create_node(std::string line){

    std::vector<std::string> string_vec = split_str(line);
    if(string_vec.size() == 0){
      return NULL;
    }
    std::string name = string_vec[0];

    FuncMap::iterator it = node_creators.find(name);
    Node * nnode = NULL;
    if(it == node_creators.end()){
      return new Node(CONTROL_GRP);
    }else{
     nnode = it->second();
    }
    
    string_vec.erase(string_vec.begin());
    nnode->handle_args(string_vec);
    return nnode;
  }

  Node * create_and_insert_node(std::string line, Vec2f pos){
    Node * nd = create_node(line);
    node_view[nd] = new NodeViewer(pos,font,line);
    backend.insert_node(nd);
    return nd;
  }



  void insert_node(Node * nd, Vec2f pos){
    node_view[nd] = new NodeViewer(pos,font);
    backend.insert_node(nd);
  }

  void connect(Node * out, int outlet, Node * in, int inlet){
    backend.connect_nodes(out,in,outlet,inlet);
  }




  Node * replace_node_with_line(std::string line, Node * node){
    Node * _new = create_node(line);
    backend.swap_node(node,_new);
    NodeViewer * nv = node_view[node];
    node_view.erase(node);
    delete node;
    node_view[_new] = nv;

  }

  void draw(){

    for(auto it = node_view.begin();it != node_view.end();it++){
      //std::cout << "Drawing node..\n";
      Node * nd = it->first;
      NodeViewer * nv = it->second;
      //std::cout << nv->body.pos.x << "\n";
      nv->sanitize(nd);
      nv->update();
      nv->draw(shader);
      
    }

    for(auto it = backend.connections.begin();it != backend.connections.end();it++){
      ConnectionInfo &cinf = *it;
      int inlet = cinf.inlet;
      int outlet = cinf.outlet;
      NodeViewer * start = node_view[cinf.out_node];
      NodeViewer * end = node_view[cinf.in_node];
      Vec2f pos_start = start->outlets[outlet].pos + Vec2f(INLET_SIZE/2,INLET_SIZE/2);
      Vec2f pos_end = end->inlets[inlet].pos + Vec2f(INLET_SIZE/2,INLET_SIZE/2);


      UiLine uil(pos_start,pos_end,{1.0,1.0,1.0,1.0});
      uil.draw(shader);

    }



  }


};

enum PORT_TYPE{
  IN_PORT,OUT_PORT,NO_PORT
};

class NodeViewerPort{
public:

  NodeViewer * node_viewer;
  PORT_TYPE port_type;
  int port_nr;
};


class NodeGridInputHandler:public NodeGrid, public EventListener<MouseClick>, public EventListener<KeyEvent>, public EventListener<mouse_position>, public EventListener<CharEvent>, public EventListener<MouseWheelEvent>{
public:

  NodeViewer * hover_nv;
  int hover_inlet;
  int hover_outlet;
  Vec2f last_world;
  NodeViewerPort nvp;
  NodeViewerPort last_nvp;
  NodeViewerPort seek_port;
  UiLine connect_line;


  NodeGridInputHandler():last_world(0.0,0.0),connect_line(Vec2f(0.0,0.0),Vec2f(0.0,0.0),{1.0,1.0,1.0,1.0}){
    last_nvp = {NULL,NO_PORT,0};
    seek_port = {NULL,NO_PORT,0};
    nvp = {NULL,NO_PORT,0};
  }

  bool handle_event(mouse_position mpos){
    Vec2f wpos = mouse_to_world(mpos);
    last_world = wpos;
    
    if(last_nvp.node_viewer != NULL){  //Handle un-hover
      if(last_nvp.port_type == NO_PORT){
	//last_nvp.node_viewer->body.box.bg = {0.0,0.0,0.0,1.0};
      }
      
    }

     nvp = find_NodeViewerPort(wpos);

     if(nvp.node_viewer != NULL){ // handle hover
      if(nvp.port_type == NO_PORT){
	//nvp.node_viewer->body.box.bg = {1.0,0.0,0.0,1.0};
      }
    }
     last_nvp = nvp;

    
   }

  


  NodeViewerPort find_NodeViewerPort(Vec2f wpos){
    NodeViewerPort nvp = {NULL,NO_PORT,0};

    
    //for_each(node_viewers.begin(),node_viewers.end(),[&](NodeViewer *& nv){
    for(auto it = node_view.begin();it != node_view.end();it++){
      NodeViewer * nv = it->second;
	if (nv->body.check_point(wpos)){
	  nvp = {nv,NO_PORT,0};
	}	
	
	for(int i = 0; i < nv->inlets.size();i++){
	  UiBox &ub = nv->inlets[i];
	  if(ub.check_point(wpos)){
	    nvp = {nv,IN_PORT,i};   
	  }
	}
	
	for(int i = 0; i < nv->outlets.size();i++){
	  UiBox &ub = nv->outlets[i];
	  if(ub.check_point(wpos)){
	    nvp = {nv,OUT_PORT,i};
	  }
	}
      }
    return nvp;
  }


  void select_node(NodeViewerPort nvp){
    if(seek_port.node_viewer != NULL){
      seek_port.node_viewer->body.box.bg = {0.0,0.0,0.0,1.0};
    }
    seek_port = nvp;
    if(seek_port.node_viewer != NULL){
      seek_port.node_viewer->body.box.bg = {0.1,0.1,0.1,1.0};
    }
  }

  bool handle_event(CharEvent ce){
    if(ce.pressed){
      if(seek_port.node_viewer != NULL && seek_port.port_type == NO_PORT){
	seek_port.node_viewer->body.text +=ce.charkey;
      }
    }
  }
  
  bool handle_event(KeyEvent ce){
    if(ce.pressed){
      if(seek_port.node_viewer != NULL && seek_port.port_type == NO_PORT){
	std::string &text = seek_port.node_viewer->body.text;
	if(ce.key == BACKSPACE){
	  text.erase(text.size()-1);
	}
	if(ce.key == ENTER){
	  Node * nd = find_node_from_nodeview(seek_port.node_viewer);
	  replace_node_with_line(text,nd);
	  select_node({NULL,NO_PORT,0});
	  
	  
	}
      }
    }
  }
  


  bool handle_event(MouseClick mclick){
    if(mclick.pressed){
      select_node({NULL,NO_PORT,0});
    }
    if(mclick.button == 0 && mclick.pressed){
      if(nvp.node_viewer != NULL){
	select_node(nvp);
	
      }else{
	seek_port = {NULL,NO_PORT,0};
      }
    }
  }

  void draw2(GLProgram & shader){
    if(seek_port.node_viewer != NULL && seek_port.port_type != NO_PORT){
      connect_line.pos_start = seek_port.node_viewer->body.pos;
      connect_line.pos_end = last_world;
      connect_line.draw(shader);
    
    }

  }
};






void test_nodeui_base(){
  init_ogl(512,512);
  init_audio(44100, 16, 1);
  shared_ptr<FTGLPixmapFont> font = new FTGLPixmapFont(FONT);
  

  GLProgram shader = make_program("shaders/naive.vert","shaders/naive.frag");
  UiBox ub1(Vec2f(-400.0,-300.0),Vec2f(800.0,600.0),{0.0,0.0,0.0,1.0},0.0,{0.0,0.0,0.0,0.0});
  
  square_vbo  = make_buffer<float>( {0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0}, 2,ARRAY_BUFFER, STATIC_DRAW);
  line_vbo = make_buffer<float>({0.0,0.0,1.0,1.0}, 2,ARRAY_BUFFER, STATIC_DRAW);
  float it = 0.0;
  scale =0.5;
  camera_x = 200.0;
  camera_y = -50.0;
  NodeGridInputHandler ngrid;
  ngrid.shader = shader;
  ngrid.node_creators["osc"] = [](){return new Osc(440);};
  ngrid.node_creators["add-audio"] = [](){return new AddAudio();};
  ngrid.node_creators["audio-out"] = [](){return new AudioOut(8*1024);};
  ngrid.node_creators["line-play"] = [](){return new line_play(1.0,std::vector<float>({0.0})) ;};
  ngrid.node_creators["phasor"] = [](){return new Phasor(440) ;};
  
  Node * n1 = ngrid.create_and_insert_node("osc 440",Vec2f(0.0,0.0));
  Node * n3 =ngrid.create_and_insert_node("audio-out",Vec2f(150.0,-150.0));
  Node * lp = ngrid.create_and_insert_node("line-play 0.001 1.0 0.0 2.0 0.0",Vec2f(-150.0,150.0));


  ngrid.connect(n1,0,n3,0);
  ngrid.connect(lp,0,n1,0);
  mouse_move_spawner.register_listener(&ngrid);
  mouse_click_handler.register_listener(&ngrid);
  char_event_spawner.register_listener(&ngrid);
  key_event_handler.register_listener(&ngrid);
  mouse_wheel_event_spawner.register_listener(&ngrid);

  out_stream o_stream(8*1024,44100);
  o_stream.init();
  o_stream.Play();
  AudioOut * ao =(AudioOut *) n3;

  while(true){
    



    for(int i = 0; i < ao->buffersize;i++){
      ngrid.backend.update();
    }
    o_stream.load_buffer(ao->buffer, ao->buffersize);
    while(o_stream.load_next == false){
      shader.uniformf("camera_scale",1.0,1.0);
      shader.uniformf("camera_translate",0.0,0.0);
      bind_buffer_object(square_vbo,0);
      shader.uniformf("size",2.0,2.0);
      shader.uniformf("pos",-1.0,-1.0);
      shader.uniformf("color",0.0,0.0,0.5,1.0);
      draw_buffers_triangle_fan(4);


      shader.uniformf("camera_scale",2.0/512.0*scale,2.0/512.0*scale);
      shader.uniformf("camera_translate",camera_x,camera_y);
      ngrid.draw();
      swapbuffers();
     sleep_sec(5.0/44100.0);
    }
    //std::cout << get_time() << "\n";
    
  }


  /*
  return;
  

  ngrid.sanitise_connections();
  ngrid.shader = shader;
  ngrid.connect(n1,0,n3,0);
  ngrid.connect(lp,0,n1,0);

  
  

  while(true){
    
    shader.uniformf("camera_scale",2.0/512.0*scale,2.0/512.0*scale);
    shader.uniformf("camera_translate",camera_x,camera_y);

    it += 0.2; 
    for(int i = 0; i < ao->buffersize;i++){
      ngrid.update();
    }
    ub1.draw(shader);
    ngrid.draw();
    ngrid.draw2(shader);

    o_stream.load_buffer(ao->buffer, ao->buffersize);
    while(o_stream.load_next == false){
      sleep_sec(5.0/44100.0);
    }
    std::cout << get_time() << "\n";
    swapbuffers();
  }

  */
}


void run_nodeui(){
  test_nodeui_base();
  return;
}
