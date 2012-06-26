#include "input_events.h"
#include "GLProgram.h"

extern int global_screen_width;
extern int global_screen_height;

extern GLProgram texture_shader;
extern GLProgram color_shader;

void set_camera_position(int x, int y);
void set_world_to_view(float scale_x, float scale_y, int view_pixels_width, int view_pixels_height);

mouse_position screen_pos_to_world_pos(mouse_position mpos);

void init_game(int window_width, int window_height,int width_pixels, int height_pixels);
void setup_shader_uniforms(GLProgram s);
