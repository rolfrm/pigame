#ifdef GL_ES
precision lowp float;
#endif

uniform float time;
uniform sampler2D tex;
uniform float deg;
varying vec2 uv;

void main(){
     //vec2 uv2 = vec2(1.0 - uv.x,1.0 - uv.y);
     int x=int(uv.x*1024.0);
     int y=int(uv.y*1024.0);
     if(mod(x,4)!=0.0 && mod(y,4)!=0.0){
	gl_FragData[0]=texture2D(tex, uv)*1.2;
	}
	else{
	gl_FragData[0]=vec4(texture2D(tex, uv).xyz*0.80,1.0);
	}
	
	
}
