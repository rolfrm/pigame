#pragma once
#include<vector>
typedef struct BufferObject{
  unsigned int gl_ref;
  int * ref_cnt;
  unsigned int type;
  unsigned int dim;
  unsigned int storetype;
}BufferObject;

#define ARRAY_BUFFER 0x8892
#define STATIC_DRAW 0x88E4
#define FLOAT 0x1406 //GL_FLOAT
#define SHORT 0x1402  //GL_SHORT
#define BYTE 0x1400  //GL_BYTE
BufferObject make_buffer_object(void * data, int n_vertexes, int vertex_dim, int type, unsigned int gl_storetype = ARRAY_BUFFER, unsigned int gl_optimization = STATIC_DRAW);

template<class T>
BufferObject make_buffer(std::vector<T> data, int vertex_dim, unsigned int gl_storetype, unsigned int gl_optimization){
  unsigned int type = BYTE;
  switch (sizeof(T)){
  case 1: type = BYTE;break;
  case 2: type = SHORT;break;
  case 4: type = FLOAT;break;
  }
  return make_buffer_object((void *) &data[0],data.size(), vertex_dim, type, gl_storetype,gl_optimization); 
} 


void bind_buffer_object(BufferObject bobj, unsigned int loc);
BufferObject cpy_buf_obj(BufferObject bobj);
void del_buf_obj(BufferObject bobj);
void buffer_object_test();

void draw_buffers_triangle_fan(int n_values);
void draw_buffers_line_loop(int n_values);
