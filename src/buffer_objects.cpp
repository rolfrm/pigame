#include "buffer_objects.h"
#include "gl_inc.h"
#include <stdlib.h>
BufferObject make_buffer_object(void * data, int n_vertexes, int vertex_dim, int type, unsigned int gl_storetype, unsigned int gl_optimization){
  BufferObject out;
  out.ref_cnt = (int * ) malloc(sizeof(unsigned int));
  *(out.ref_cnt) = 1;
  out.type = type;
  out.dim = vertex_dim;
  out.storetype = gl_storetype;
  unsigned int type_size;
  if(type == GL_UNSIGNED_INT || type == GL_INT || type == GL_FLOAT){
    type_size = 4;
  }else if(type == GL_SHORT){
    type_size = 2;
  }else{
    type_size = 1;
  }
  glGenBuffers(1,&(out.gl_ref));
  glBindBuffer(gl_storetype, out.gl_ref);
  glBufferData(gl_storetype, n_vertexes*vertex_dim*type_size, data, gl_optimization);
  return out;
} 

void bind_buffer_object(BufferObject bobj, unsigned int loc){
  glBindBuffer(bobj.storetype, bobj.gl_ref);
  glVertexAttribPointer(loc, bobj.dim, bobj.type, 0, 0, 0);
  glEnableVertexAttribArray(loc);
}

BufferObject cpy_buf_obj(BufferObject bobj){
  *(bobj.ref_cnt) += 1;
  return bobj;
}

void del_buf_obj(BufferObject bobj){
  *(bobj.ref_cnt) -=1;
  if(*(bobj.ref_cnt) <= 0){
    glDeleteBuffers(1,&(bobj.gl_ref));
    free(bobj.ref_cnt);
  }
}

void buffer_object_test(){
  float data[12] = {1,2,3,4,5,6,7,8,9,10,11,12};
  BufferObject bobj = make_buffer_object(data,4,3,GL_FLOAT,GL_ARRAY_BUFFER,GL_STATIC_DRAW);
  BufferObject bobj2 = cpy_buf_obj(bobj);
  bind_buffer_object(bobj2,0);
  del_buf_obj(bobj);
  del_buf_obj(bobj2);

}

void draw_buffers_triangle_fan(int n_values){
  glDrawArrays(GL_TRIANGLE_FAN,0,n_values);
}
