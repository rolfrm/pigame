#include "texture.h"
#include <vector>

class FrameBuffer{
 public:
  unsigned int gl_ref;
  int * ref_cnt;
  std::vector<Texture> textures;
  FrameBuffer(Texture tex);
  FrameBuffer(std::vector<Texture> texvec);
};

void bind_framebuffer(FrameBuffer fb);
void unbind_framebuffer();
void clear_bound_framebuffer();
