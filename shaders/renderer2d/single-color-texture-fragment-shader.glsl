#version 100

precision mediump float;

varying vec2 textureCoord;

uniform sampler2D tex;

uniform float alpha;

uniform vec3 color;

uniform int facing;

vec2 texCoord;

vec4 textureColor;

void main(){

	//gl_FragColor = vec4(1.0, 0.5, 0.0, 1.0);

	texCoord = textureCoord;

	if(facing == 0){
		texCoord.x = 1.0 - texCoord.x;
	}

	textureColor = texture2D(tex, texCoord);

	gl_FragColor = vec4(color.x, color.y, color.z, textureColor.w * alpha);

	if(textureColor.x == 0.0
	&& textureColor.y == 0.0
	&& textureColor.z == 0.0
	&& textureColor.w == 1.0){
		gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
	}

}
