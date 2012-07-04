#pragma once
struct collision_info{
	float axis[2],overlap;
	int from;
};

struct AABB{
  float x,y;
  float size_x,size_y;
  bool movable;
  bool ghost;
};

int AABB_collision_detection2(AABB * a, AABB * b,float & overlap); //returns -1(no collision), 0(x-collision), 1(y-collision)

bool AABB_collision_detection(float * aabb1,float * aabb2);

collision_info AABB_seperating_axis_collision_detection(float * aabb1,float * aabb2);

collision_info seperating_axis_collision_detection(float * vertex1,float * axis1,int n_vertex1,float * vertex2,float * axis2,int n_vertex2);

void calculate_axis(float * vertex,float * axis,int n_vertex);

void calculate_AABB(float * vertex,float * aabb,int n_vertex);

void is_clockwise(float * vertex,int n_vertex);
