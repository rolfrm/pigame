#ifdef GL_ES
precision lowp float;
#endif

uniform float time;
uniform sampler2D tex;
uniform float deg;
varying vec2 uv;

void main(){
	vec2 dis=vec2(0.5,0.5)-uv;
	//gl_FragData[0]=vec4(1.0,1.0,1.0,1.0)*exp(-length(dis*5.0)-cos(time)*0.1);
	//if(time<0.0)
	if(dis.x *dis.x + dis.y*dis.y < 0.2 || deg < 0.1){
		gl_FragData[0]=vec4(uv,time,1.0)*deg + texture2D(tex,uv)*(1.0 - deg);
	}
	//else
	//	gl_FragData[0]=vec4(1.0);
}
