#include "texture.h"
#include <vector>

class FrameBuffer{
 public:
  unsigned int gl_ref;
  int * ref_cnt;
  std::vector<Texture> textures;
  GLuint stencil_buffer;
  FrameBuffer(Texture tex);
  FrameBuffer(std::vector<Texture> texvec);
  FrameBuffer(int width,int heigth,int colorChannels, int interp_param = 0x2600, int wrap_param = 0x2901);
};

void bind_framebuffer(FrameBuffer fb);
void unbind_framebuffer();
void clear_bound_framebuffer();
