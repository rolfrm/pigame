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
#include <iostream>
#include <fstream>
#include <thread>
extern int window_width;
extern int window_height;
extern float camera_x;
extern float camera_y;
float scale = 0.25;
//#define FONT "/var/lib/defoma/x-ttcidfont-conf.d/dirs/TrueType/Courier_New.ttf"
#define FONT "/usr/share/fonts/truetype/ubuntu-font-family/UbuntuMono-B.ttf"

#define BUFFERSIZE 256
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

class Scheduler{
public:
  int index;
  int tick;
  float cnt;
  std::list<Node *> nodes;

  Scheduler(int _index,int _tick){
    cnt = 0.0;
    tick = _tick;
    index = _index;
  }
  
  void update_nodes(){
    for(auto it = nodes.begin(); it != nodes.end();it++){
      (*it)->update();
    }
  }

  virtual void do_update(){
    update_nodes();
  }
};



class NodeBackend{
public:
  std::list<ConnectionInfo> connections;
  std::list<Node *> nodes;
  std::map<int, Scheduler *> sched_list;
  float t;

  NodeBackend(){
    t = get_time();
  }

  void insert_node(Node * nd){
    nodes.push_back(nd);
    if(sched_list.find(nd->grp) != sched_list.end()){
      std::cout << "found: " << nd->grp << "\n";
      sched_list[nd->grp]->nodes.push_back(nd);
      }
  }

  void register_scheduler(Scheduler * sched){
    sched_list[sched->index] = sched;
  }

  void remove_connection(std::list<ConnectionInfo>::iterator & it){

    std::vector<_input *> &ve = it->out_node->outputs[it->outlet];
    for(auto it2 = ve.begin();it2 != ve.end();it2++){
      if(*it2 == it->in_node->inputs[it->inlet]){
	ve.erase(it2);
	return;
      }
    }
    connections.erase(it);

  }


  void remove_node(Node *nd){
    nodes.remove(nd);
    sched_list[nd->grp]->nodes.remove(nd);
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
    sched_list[old->grp]->nodes.remove(old);
    nodes.push_back(_new);
    sched_list[_new->grp]->nodes.push_back(_new);
    std::list<ConnectionInfo> new_connections;
    for(auto it = connections.begin();it != connections.end();it++){
      if(it->out_node == old){
	it->out_node = _new;
	
	if(it->outlet >= 0 && it->outlet < _new->outputs.size()){
	  _new->outputs[it->outlet].push_back(it->in_node->inputs[it->inlet]);
	}
      }
      if(it->in_node == old){
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
      
      float t_now = get_time();
      float dt = t_now - t;
      t = t_now;
      for(auto it = sched_list.begin();it != sched_list.end();it++){
	if(it->second->tick != 0){
	  
	it->second->cnt += dt;
	while(it->second->cnt > (float) 1.0/ (float)it->second->tick){
	  it->second->do_update();
	  it->second->cnt -= (float) 1.0/ (float)it->second->tick;
	}
	}
	
      }
      sleep_sec(0.001);
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
  shared_ptr<FTFont> font;
  std::string text;
  float padding;
  float fontsize;
  UiTextBox(Vec2f _pos,shared_ptr<FTFont> _font):font(_font),box(_pos,Vec2f(0,0),{0.0,0.0,0.0,1.0},1.0,{1.0,1.0,1.0,1.0}), pos(_pos){
    padding = 30.0;
    fontsize = 32;
  }

  bool check_point(Vec2f point){
    return box.check_point(point);
  }

  void draw(GLProgram &s){
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
  NodeViewer(Vec2f pos,shared_ptr<FTFont> fnt,std::string text = ""):body(pos,fnt){
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

  FuncMap node_creators;
  NodeView node_view;
  shared_ptr<FTFont> font;
  GLProgram shader;
  NodeBackend backend;
  bool * pause_signal;
  bool * got_pause;


  NodeGrid(bool * _pause_signal, bool * _got_pause){
    font = new FTGLBitmapFont(FONT);
    pause_signal = _pause_signal;
    got_pause = _got_pause;
  }

  void get_pause(){

    *pause_signal = true;
    while(!*got_pause){
      sleep_sec(1.0/1000000.0);
    }
  }

  void release_pause(){
    *pause_signal = false;
  }

  virtual void clear_local(){};

  Node * find_node_from_nodeview(NodeViewer * nv){
    for(auto it = node_view.begin();it != node_view.end();it++){
      if(it->second == nv){
	return it->first;
      }
    }
    return NULL;

  }
  void delete_node(NodeViewer * nv){
    Node * nd = find_node_from_nodeview(nv);
    std::cout << nd << "<<- \n";
    get_pause();
    node_view.erase(nd);
    backend.remove_node(nd);
    delete nd;
    delete nv;
    release_pause();

  }

  Node * create_node(std::string line){

    std::vector<std::string> string_vec = split_str(line);
    if(string_vec.size() == 0){
      return new Node(CONTROL_GRP);
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
    std::cout << "Replacing node with " << line << "\n";
    get_pause();
    Node * _new = create_node(line);
    backend.swap_node(node,_new);
    NodeViewer * nv = node_view[node];
    node_view.erase(node);
    delete node;
    node_view[_new] = nv;
    release_pause();

  }

  void draw(){

    for(auto it = node_view.begin();it != node_view.end();it++){
      Node * nd = it->first;
      NodeViewer * nv = it->second;
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
  
  /*Save program data format
    n_nodes
    line x y
    line x y
    ...
    n_connections
    out port in port
    out port in port
   */

  void save_program(std::string path){
    std::ofstream cout(path, std::ios::out|std::ios::trunc);

    cout << node_view.size() << "\n";
    for(auto it = node_view.begin(); it != node_view.end();it++){
      NodeViewer * nv = it->second;
      cout << nv->body.text << " " << nv->body.pos.x << " " << nv->body.pos.y << "\n";
    }
    auto find_index = [&node_view](NodeViewer *nv){
      int i = 0;
      for(auto it = node_view.begin();it != node_view.end();it++){
	if(it->second == nv){
	  return i;
	}
	i++;
      }
      return -1;
    };
    cout << backend.connections.size() << "\n";
    for(auto it = backend.connections.begin(); it != backend.connections.end();it++){
      NodeViewer * nv1 = node_view[it->out_node];
      int out_port = it->outlet;
      NodeViewer * in = node_view[it->in_node];
      int in_port = it->inlet;
      cout << find_index(nv1) << " " << out_port << " " << find_index(in) << " " << in_port << "\n";

    }
    cout.close();
  }

  void load_program(std::string path){
    get_pause();
    while(node_view.size() > 0){
      delete_node(node_view.begin()->second);
    }
    clear_local();


    std::ifstream cin(path, std::ios::in);
    char buffer[100];
    std::string str;
    int state = 0;
    std::getline(cin,str);
    int n_nodes = lexical_cast<int,std::string>(str);
    std::cout << "N Nodes:" << n_nodes << "\n";
    std::vector<Node *> nodes;

    for(int i = 0; i < n_nodes;i++){
      std::getline(cin,str);
      std::vector<std::string> args = split_str(str);
      float y = lexical_cast<float,std::string>(args[args.size()-1]);
      args.pop_back();
      float x = lexical_cast<float,std::string>(args[args.size()-1]);
      args.pop_back();
      std::string full_name;
      for(int i = 0; i < args.size();i++){
	full_name += args[i];
	if(i != args.size()-1){
	  full_name += " ";
	}
      }
      std::cout << full_name << "\n";
      nodes.push_back(create_and_insert_node(full_name,Vec2f(x,y)));
    }

    std::getline(cin,str);
    int n_connections = lexical_cast<int,std::string>(str);
    for(int i = 0; i < n_connections;i++){
      std::getline(cin,str);
      std::vector<std::string> args = split_str(str);
      int out_node = lexical_cast<int,std::string>(args[0]);
      int out_port = lexical_cast<int,std::string>(args[1]);
      int in_node = lexical_cast<int,std::string>(args[2]);
      int in_port = lexical_cast<int,std::string>(args[3]);
      backend.connect_nodes(nodes[out_node],nodes[in_node],out_port,in_port);

    }
    release_pause();
    cin.close();
    std::cout << "Done loading.. \n";
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

enum UI_MODE{
  EDIT_MODE,
  INTERACT_MODE};

class NodeGridInputHandler:public NodeGrid, public EventListener<MouseClick>, public EventListener<KeyEvent>, public EventListener<mouse_position>, public EventListener<CharEvent>, public EventListener<MouseWheelEvent>{
public:

  Vec2f last_world;
  NodeViewerPort nvp;
  NodeViewerPort last_nvp;
  NodeViewerPort seek_port;
  NodeViewerPort held_node;
  UiLine connect_line;
  bool middle_down;
  bool ctrl_down;
  bool shift_down;
  mouse_position last_pos;
  UI_MODE mode;

  
  
  bool change;

  NodeGridInputHandler(bool *pause_proc, bool * pause_ready):last_world(0.0,0.0),connect_line(Vec2f(0.0,0.0),Vec2f(0.0,0.0),{1.0,1.0,1.0,1.0}),NodeGrid(pause_proc,pause_ready){
    change = true;
    pause_signal = pause_proc;
    got_pause = pause_ready;
    no_write_it = 0;
    last_nvp = {NULL,NO_PORT,0};
    seek_port = {NULL,NO_PORT,0};
    held_node = {NULL,NO_PORT,0};
    nvp = {NULL,NO_PORT,0};
    middle_down = false;
    ctrl_down = false;
    shift_down = false;
    mode = EDIT_MODE;
  }
  void clear_local(){
    nvp = {NULL,NO_PORT,0};
    last_nvp = nvp;
    seek_port = nvp;
    held_node = {NULL,NO_PORT,0};
    
  }


  bool handle_event(mouse_position mpos){
    change = true;
    Vec2f wpos = mouse_to_world(mpos);
    if(held_node.node_viewer != NULL && held_node.port_type == NO_PORT){
      held_node.node_viewer->body.pos.x += (mpos.x - last_pos.x)/scale;
      held_node.node_viewer->body.pos.y -= (mpos.y - last_pos.y)/scale;
      
    }


    if(middle_down){
      camera_x -= (mpos.x - last_pos.x)/scale;
      camera_y += (mpos.y - last_pos.y)/scale;
    }

    last_pos = mpos;

    last_world = wpos;
    
    if(last_nvp.node_viewer != NULL){  //Handle un-hover
      if(last_nvp.port_type == NO_PORT){
	last_nvp.node_viewer->body.box.bg = {0.0,0.0,0.0,1.0};
      }
      
    }

     nvp = find_NodeViewerPort(wpos);

     if(nvp.node_viewer != NULL){ // handle hover
      if(nvp.port_type == NO_PORT){
	nvp.node_viewer->body.box.bg = {0.1,0.1,0.1,1.0};
      }
    }
     last_nvp = nvp;

    
   }

  NodeViewerPort find_NodeViewerPort(Vec2f wpos){
    NodeViewerPort nvp = {NULL,NO_PORT,0};

    
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

  std::string saved_text;
  int curser_pos;
  int no_write_it;
  bool text_changed;
  void select_node(NodeViewerPort nvp){
    if(seek_port.node_viewer != NULL && seek_port.port_type == NO_PORT){
      seek_port.node_viewer->body.box.bg = {0.0,0.0,0.0,1.0};
      seek_port.node_viewer->body.text = saved_text;
    }
    seek_port = nvp;
    if(seek_port.node_viewer != NULL&& seek_port.port_type == NO_PORT){
      text_changed = false;
      seek_port.node_viewer->body.box.bg = {0.1,0.1,0.1,1.0};
      saved_text = seek_port.node_viewer->body.text;
      curser_pos = saved_text.size();
      saved_text.insert(curser_pos,"|");
      seek_port.node_viewer->body.text = saved_text;
    }
  }

  void unselect_node(){
    if(seek_port.port_type == NO_PORT && seek_port.node_viewer != NULL){
      Node * nd = find_node_from_nodeview(seek_port.node_viewer);
      saved_text.erase(curser_pos,1);
      if(text_changed == true){
	replace_node_with_line(saved_text,nd);
      }
    }
    
     select_node({NULL,NO_PORT,0});
  }

  void select_port(NodeViewerPort nvp){
    
  }


  bool handle_event(CharEvent ce){
    change = true;
    if(ce.pressed){

      if(seek_port.node_viewer != NULL && seek_port.port_type == NO_PORT){
	text_changed = true;
	saved_text.erase(curser_pos,1);

	saved_text.insert(curser_pos,1,ce.charkey);
	
	curser_pos +=1;
	saved_text.insert(curser_pos,"|");


	seek_port.node_viewer->body.text = saved_text;
      }
    }
  }
  
  

  bool handle_event(KeyEvent ce){
    change = true;
    std::cout << ce.key << "\n";

    if(ce.key == CTRL){
      ctrl_down = ce.pressed;
      return false;
    }
    
    if(ce.key == SHIFT){
      shift_down = ce.pressed;
      return false;
    }


    if(ce.key == ARROW_LEFT || ce.key == ARROW_RIGHT){
      
      saved_text.erase(curser_pos,1);
      if(ce.key == ARROW_LEFT){
	if(curser_pos > 0){
	  curser_pos -=1;
	}
      }

      if(ce.key == ARROW_RIGHT){
	if(curser_pos < saved_text.size()){
	  curser_pos +=1;
	}
      }
      
          saved_text.insert(curser_pos,"|");
	  seek_port.node_viewer->body.text = saved_text;
	  return false;
    }
    
    if(ce.pressed){

      if(ce.key == TAB){
	mode = mode == EDIT_MODE? INTERACT_MODE : EDIT_MODE;
	return false;
      }

      if(ce.key == DELETE){
	if(seek_port.node_viewer != NULL && seek_port.port_type == NO_PORT){
	  
	  delete_node(seek_port.node_viewer);
	  
	}
	unselect_node();
	return false;
      }

      if(seek_port.node_viewer != NULL && seek_port.port_type == NO_PORT){
	
	if(ce.key == BACKSPACE){
	  text_changed = true;
	  if(curser_pos >= 1){
	    saved_text.erase(curser_pos-1,1);
	    curser_pos -=1;
	  }
	  
	}

	seek_port.node_viewer->body.text = saved_text;
	if(ce.key == ENTER){
	  unselect_node();
	  /*saved_text.erase(curser_pos,1);
	  Node * nd = find_node_from_nodeview(seek_port.node_viewer);
	  
	  replace_node_with_line(saved_text,nd);
	  
	  select_node({NULL,NO_PORT,0});
	  */
	  
	}
      }
    }
  }
  
  void remove_connection_at(Vec2f world_pos){
    float threshold = 20.0;
    for(auto it = backend.connections.begin();it != backend.connections.end();it++){
      ConnectionInfo &cinf = *it;
      int inlet = cinf.inlet;
      int outlet = cinf.outlet;
      NodeViewer * start = node_view[cinf.out_node];
      NodeViewer * end = node_view[cinf.in_node];
      Vec2f pos_start = start->outlets[outlet].pos + Vec2f(INLET_SIZE/2,INLET_SIZE/2);
      Vec2f pos_end = end->inlets[inlet].pos + Vec2f(INLET_SIZE/2,INLET_SIZE/2);
      if(line_point_distance(pos_start.x,pos_start.y,pos_end.x,pos_end.y, world_pos.x,world_pos.y) < threshold){
	backend.remove_connection(it);
	return;
      }
      }

      
      

    }
  

  bool handle_event(MouseClick mclick){
    change = true;

    if(mclick.button == 2){
      middle_down = mclick.pressed;
    }

    if(mode == INTERACT_MODE){
      NodeViewer * nv = nvp.node_viewer;
      if(nv == NULL){
	return false;
      }
      if(mclick.button == 0 && mclick.pressed){
	Node * nd = find_node_from_nodeview(nv);
	if(nd != NULL){
	  nd->handle_click(0,0,0);
	}else{
	  std::cout << "Warning. Clicking NULL node\n";
	}
      }


    }


    if(mode == EDIT_MODE){
      if(shift_down){
	remove_connection_at(last_world);
	return false;
      }

      if(ctrl_down && mclick.button == 0 && mclick.pressed){
	Node * nd = create_and_insert_node("",last_world);
	held_node = {node_view[nd],NO_PORT,0};
	return false;
      }

      if(mclick.button == 0 && mclick.pressed){
	
	held_node = nvp;
	
	return false;
      }


      if(mclick.button == 0 && mclick.released){
	if(held_node.node_viewer != NULL){
	  if( seek_port.node_viewer != NULL && seek_port.port_type != NO_PORT){
	    Node * nd1 = find_node_from_nodeview(nvp.node_viewer);
	    int p1 = nvp.port_nr;
	    Node * nd2 = find_node_from_nodeview(seek_port.node_viewer);
	    int p2 = seek_port.port_nr;
	    if(nvp.port_type == IN_PORT && seek_port.port_type == OUT_PORT){
	      backend.connect_nodes(nd2,nd1,p2,p1);
	  
	    }else if(nvp.port_type == OUT_PORT && seek_port.port_type == IN_PORT){
	      backend.connect_nodes(nd1,nd2,p1,p2);	  
	    }
	    seek_port = {NULL,NO_PORT,0};
	  }else{
	    unselect_node();
	    select_node(held_node);
	    held_node = {NULL,NO_PORT,0};
	    return false;
	  }
	  
	}else{
	  unselect_node();
	  seek_port = {NULL,NO_PORT,0};
	}
	
      }
    }
    unselect_node();
    
  }

  bool handle_event(MouseWheelEvent mve){
    change = true;
    scale *= 1 + ((float)mve.change)*0.1;
    return false;
  }

  void draw2(GLProgram & shader){
    no_write_it += 1;
    
    if(seek_port.node_viewer != NULL && seek_port.port_type != NO_PORT){
      if(seek_port.port_type == IN_PORT){
	connect_line.pos_start = seek_port.node_viewer->inlets[seek_port.port_nr].pos + Vec2f(INLET_SIZE/2,INLET_SIZE/2);
      }else{
	connect_line.pos_start = seek_port.node_viewer->outlets[seek_port.port_nr].pos+ Vec2f(INLET_SIZE/2,INLET_SIZE/2);
      }
      connect_line.pos_end = last_world;
      connect_line.draw(shader);
    
    }

  }
};

class MultNode: public Node{
public:
  input<audio> i1,i2;
  MultNode():Node(CONTROL_GRP){
    register_input(i1);
    register_input(i2);
    register_output<audio>();
  }
  void update(){
    audio f = sqrt(i1.value * i2.value);
    push_output(0,f);
  }
};

class DivNode: public Node{
public:
  input<audio> i1,i2;
  DivNode():Node(CONTROL_GRP){
    register_input(i1);
    register_input(i2);
    register_output<audio>();
  }
  void update(){
    audio f = i1.value / i2.value;
    push_output(0,f);
  }
};

class ClipNode: public Node{
public:
  input<audio> i1;
  input<float> i2;
  ClipNode():Node(CONTROL_GRP){
    register_input(i1);
    register_input(i2);
    register_output<audio>();
  }
  void update(){
    if(i1.value > i2.value){
      push_output(0,(audio)i2.value);
    }else if(i1.value < -i2.value){
      push_output(0,(audio)-i2.value);
    }else{
      push_output(0,(audio)i1.value);
    }

  }
};

class AudioOut2: public Node{
public:
  std::vector<input<audio>> inputs;
  int n_channels;
  AudioOut2(int channels):Node(CONTROL_GRP){
    n_channels = channels;
    for(int i = 0; i< channels;i++){
      inputs.push_back(input<audio>());
      register_input(inputs[i]);
    }
  }

  void update(){

  }
};


class QuitProgramNode: public Node{
public:
  bool * running_state;

  QuitProgramNode(bool * running):Node(CONTROL_GRP){
    running_state = running;
  }

  void handle_click(float internal_x,float internal_y, bool keystate){
    *running_state = false;
  }
};

class SavePatchNode: public Node{

public:
  std::string save_pos;
  NodeGrid *ng;
  SavePatchNode(NodeGrid * _ng):Node(CONTROL_GRP){
    ng = _ng;
  }

  void handle_args(std::vector<std::string> args){
    if(args.size() > 0){
      save_pos = args[0];
    }
  }
  void handle_click(float x, float y, bool keystate){
    if(save_pos == ""){
      return;
    }
    std::cout << "Saving to " << save_pos << "\n";
    ng->save_program(save_pos);
  }

};

class LoadPatchNode: public Node{

public:
  std::string save_pos;
  NodeGrid *ng;
  LoadPatchNode(NodeGrid * _ng):Node(CONTROL_GRP){
    ng = _ng;
  }

  void handle_args(std::vector<std::string> args){
    if(args.size() > 0){
      save_pos = args[0];
    }
  }
  void handle_click(float x, float y, bool keystate){
    if(save_pos == ""){
      return;
    }
    std::cout << "Loading from " << save_pos << "\n";
    ng->load_program(save_pos);
  }
};

class AudioOut3: public Node{
public:
  static audio shared_buffer[BUFFERSIZE];
  static audio shared_buffer2[BUFFERSIZE];
  int it;
  input<audio> i1;
  input<audio> i2;
  AudioOut3():Node(AUDIO_GRP){
    it = 0;
    register_input(i1);
    register_input(i2);
  }
  void update(){
    shared_buffer[it] += i1.value;
    shared_buffer2[it] += i2.value;
    it +=1;
    it = it % BUFFERSIZE;
  }

};
audio AudioOut3::shared_buffer[BUFFERSIZE];
audio AudioOut3::shared_buffer2[BUFFERSIZE];
class AudioScheduler: public Scheduler{
public:
  static bool do_next;

  AudioScheduler():Scheduler(AUDIO_GRP,0){
    
  }


  void do_update(){
    
  }

};

bool AudioScheduler::do_next = true;

class ControlScheduler:public Scheduler{
public:
  ControlScheduler():Scheduler(CONTROL_GRP,100){
    
  }

};



#include <RtAudio.h>
int saw( void * outputBuffer, void *inputBuffer, unsigned int nBufferFrames, double streamTime,
	 RtAudioStreamStatus status, void * userdata){
  

  AudioScheduler * ao = (AudioScheduler *) userdata;

  for(int i = 0; i < nBufferFrames;i++){
     ao->update_nodes();
  }

  for(int i = 0; i < nBufferFrames;i++){
    ((short*) outputBuffer)[i*2] = AudioOut3::shared_buffer[i];
    ((short*) outputBuffer)[i*2+1] = AudioOut3::shared_buffer2[i];
    AudioOut3::shared_buffer[i] = 0;
    AudioOut3::shared_buffer2[i] = 0;
  }
  AudioScheduler::do_next = true;
}


void test_nodeui_base(){
  
  RtAudio dac;
  RtAudio::StreamParameters parm;
  parm.deviceId = 0;//dac.getDefaultOutputDevice();
  parm.nChannels = 2;
  parm.firstChannel = 0;

  RtAudio::StreamOptions so;
  
  unsigned int bufferFrames = BUFFERSIZE;
  AudioScheduler a_sched;
  ControlScheduler c_sched;

  dac.openStream(&parm,NULL,RTAUDIO_SINT16, 44100, &bufferFrames, & saw, (void *) &a_sched);
  dac.startStream();
  
  
  init_ogl(1024,1024);
  

  GLProgram shader = make_program("shaders/naive.vert","shaders/naive.frag");
  UiBox ub1(Vec2f(-400.0,-300.0),Vec2f(800.0,600.0),{0.0,0.0,0.0,1.0},0.0,{0.0,0.0,0.0,0.0});
  
  square_vbo  = make_buffer<float>( {0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0}, 2,ARRAY_BUFFER, STATIC_DRAW);
  line_vbo = make_buffer<float>({0.0,0.0,1.0,1.0}, 2,ARRAY_BUFFER, STATIC_DRAW);
  float it = 0.0;
  scale =0.5;
  camera_x = 200.0;
  camera_y = -50.0;

  bool running = true;
  bool pause_proc = false;
  bool pause_ready = false;
  NodeGridInputHandler ngrid(&pause_proc, &pause_ready);;
  ngrid.backend.register_scheduler(&a_sched);
  ngrid.backend.register_scheduler(&c_sched);
  ngrid.shader = shader;
  ngrid.node_creators["osc"] = [](){return new Osc(440);};
  ngrid.node_creators["add-audio"] = [](){return new AddAudio();};
  ngrid.node_creators["audio-out"] = [](){return new AudioOut3();};
  ngrid.node_creators["line-play"] = [](){return new line_play(1.0,std::vector<float>({0.0})) ;};
  ngrid.node_creators["phasor"] = [](){return new Phasor(440) ;};
  ngrid.node_creators["multiply"] = []() {return new MultNode();};
  ngrid.node_creators["sub-audio"] = [](){return new SubAudio();};
  ngrid.node_creators["divide"] = []() {return new DivNode();};
  ngrid.node_creators["clip"] = []() {return new ClipNode();};
  ngrid.node_creators["quit-program"] = [&running](){return new QuitProgramNode(&running);};
  ngrid.node_creators["save-patch"] = [&ngrid](){return new SavePatchNode(&ngrid);};
  ngrid.node_creators["load-patch"] = [&ngrid](){return new LoadPatchNode(&ngrid);};
  Node * n1 = ngrid.create_and_insert_node("osc", Vec2f(0.0,0.0));
  Node *n2 = ngrid.create_and_insert_node("audio-out", Vec2f(0.0,50.0));
  Node *n3 = ngrid.create_and_insert_node("line-play 0.01 10 12 10 8",Vec2f(0.0,-100));
  ngrid.connect(n1,0,n2,0);
  ngrid.connect(n3,0,n1,0);
  mouse_move_spawner.register_listener(&ngrid);
  mouse_click_handler.register_listener(&ngrid);
  char_event_spawner.register_listener(&ngrid);
  key_event_handler.register_listener(&ngrid);
  mouse_wheel_event_spawner.register_listener(&ngrid);
  
  std::thread ngrid_thread([&](){
      while(running){
	
	if(pause_proc){
	  pause_ready = true;
	}else{
	  pause_ready = false;
	  ngrid.backend.update();
	}
	
      }
    });

  while(running){
    float t = get_time();
    if(ngrid.change){
      shader.uniformf("camera_scale",1.0,1.0);
      shader.uniformf("camera_translate",0.0,0.0);
      bind_buffer_object(square_vbo,0);
      shader.uniformf("size",2.0,2.0);
      shader.uniformf("pos",-1.0,-1.0);
      shader.uniformf("color",0.0,0.0,0.5,1.0);
      draw_buffers_triangle_fan(4);


      shader.uniformf("camera_scale",2.0/1024.0*scale,2.0/1024.0*scale);
      shader.uniformf("camera_translate",camera_x,camera_y);
      ngrid.draw();
      ngrid.draw2(shader);
      ngrid.change = false;
    }else{
      std::cout << "Graphics sleeping.. \n";
    }
    swapbuffers();
    std::cout << "DT: " << get_time() - t << "\n";
    float t_left = 1.0/30.0 - (get_time() - t);
    std::cout << t_left << "\n";
    if(t_left > 0){
      sleep_sec(t_left);
    }
    
  }

  ngrid_thread.join();

}


void run_nodeui(){
  test_nodeui_base();
  return;
}
