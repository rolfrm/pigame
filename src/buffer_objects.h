#pragma once
#include "gl_inc.h"

typedef struct BufferObject{
  unsigned int gl_ref;
  int * ref_cnt;
  unsigned int type;
  unsigned int dim;
  unsigned int storetype;
}BufferObject;

BufferObject make_buffer_object(void * data, int n_vertexes, int vertex_dim, int type, unsigned int gl_storetype = GL_ARRAY_BUFFER, unsigned int gl_optimization = GL_STATIC_DRAW);
void bind_buffer_object(BufferObject bobj, unsigned int loc);
BufferObject cpy_buf_obj(BufferObject bobj);
void del_buf_obj(BufferObject bobj);
void buffer_object_test();
