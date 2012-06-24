#ifdef GL_ES
precision lowp float;
#endif

uniform float time;
uniform sampler2D tex;
uniform float deg;
varying vec2 uv;

void main(){
     //vec2 uv2 = vec2(1.0 - uv.x,1.0 - uv.y);
	gl_FragData[0]=texture2D(tex, uv);
	
}
