#ifdef GL_ES
precision lowp float;
#endif

uniform float time;
uniform sampler2D sheet;

varying vec2 uv;

void main(){
	gl_FragData[0]=texture2D(sheet,uv);
	//gl_FragData[0]=vec4(uv,0.0,1.0);
}
