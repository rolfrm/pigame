uniform sampler2D shadowTexture;
varying vec2 uv;

void main(){
	if(texture2D(shadowTexture,uv).g>0.0)
		discard;
	else
		gl_FragData[0]=vec4(vec3(0.0),0.3);

} 	
