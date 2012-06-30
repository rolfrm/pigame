#pragma once
#include "input_events.h"
#include "GLProgram.h"
#include "buffer_objects.h"
extern int global_screen_width;
extern int global_screen_height;

extern unsigned int global_width;
extern unsigned int global_height;

extern float camera_x;
extern float camera_y;

extern GLProgram texture_shader;
extern GLProgram color_shader;
extern GLProgram screen_drawer;

extern BufferObject unit_rectangle_verts;
extern BufferObject unit_rectangle_uvs;
extern BufferObject unit_rectangle_inverse_uvs;
void set_camera_position(int x, int y);
void move_camera(int dx, int dy);
void set_world_to_view(float scale_x, float scale_y, int view_pixels_width, int view_pixels_height);

mouse_position screen_pos_to_world_pos(mouse_position mpos);

void init_game(int window_width, int window_height,int width_pixels, int height_pixels);
void setup_shader_uniforms(GLProgram s);
