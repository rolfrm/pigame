#pragma once

typedef struct BufferObject{
  unsigned int gl_ref;
  int * ref_cnt;
  unsigned int type;
  unsigned int dim;
  unsigned int storetype;
}BufferObject;

#define ARRAY_BUFFER 0x8892
#define STATIC_DRAW 0x88E4
#define FLOAT 0x1406
BufferObject make_buffer_object(void * data, int n_vertexes, int vertex_dim, int type, unsigned int gl_storetype = ARRAY_BUFFER, unsigned int gl_optimization = STATIC_DRAW);
void bind_buffer_object(BufferObject bobj, unsigned int loc);
BufferObject cpy_buf_obj(BufferObject bobj);
void del_buf_obj(BufferObject bobj);
void buffer_object_test();

void draw_buffers_triangle_fan(int n_values);
