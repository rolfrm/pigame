#ifdef GL_ES
precision lowp float;
#endif


attribute vec2 Pos;
attribute vec2 UV_coord;

uniform vec2 off;
uniform vec2 scale;
uniform vec2 object_scale;
uniform vec2 camera;
varying vec2 uv;

void main(){
	uv=UV_coord;
	gl_Position = vec4(Pos,0.0,1.0);

}

