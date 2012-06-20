#ifdef GL_ES
precision lowp float;
#endif

uniform float time;

varying vec2 uv;

void main(){
	//vec2 dis=vec2(0.5,0.5)-uv;
	//gl_FragData[0]=vec4(1.0,1.0,1.0,1.0)*exp(-length(dis*5.0)-cos(time)*0.1);
	if(time<0.0)
		gl_FragData[0]=vec4(uv,time,1.0);
	else
		gl_FragData[0]=vec4(1.0);
}
