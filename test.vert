#ifdef GL_ES
precision lowp float;
#endif


attribute vec2 Pos;
attribute vec2 UV_coord;

uniform vec2 off;
uniform float scale;
varying vec2 uv;

void main(){
	uv=UV_coord;
	gl_Position = vec4(Pos*scale+off*0.01,0.0,1.0);
}

