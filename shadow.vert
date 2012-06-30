#ifdef GL_ES
precision lowp float;
#endif


attribute vec2 Pos;
attribute vec2 UV_coord;

uniform vec2 off;
uniform vec2 scale;
uniform vec2 object_scale;

uniform vec2 uv_scale;
uniform vec2 uv_offset;

uniform vec2 camera;

uniform vec2 lDirection;

vec3 Light=vec3(lDirection,-1.0);

varying vec2 uv;

void main(){
	uv=UV_coord*uv_scale+uv_offset;
	if(Pos.y==1.0){
		gl_Position = vec4((Pos*object_scale + off -camera)*scale,0.0,1.0);
	}
	else{
		vec3 p=vec3(Pos.x,0.0,Pos.y);
		float t=-2.0/Light.z;
		vec3 inter=Light*t+p;
		gl_Position = vec4((vec2(inter.x,inter.y+1.0)*object_scale + off - camera)*scale,0.0,1.0);
	}
	//gl_Position = vec4((Pos*object_scale + off)*scale,0.0,1.0);
}

