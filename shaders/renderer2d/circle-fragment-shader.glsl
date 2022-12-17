#version 100

precision mediump float;

uniform float alpha;

uniform vec3 color;

varying vec2 vertexPosition;

void main(){

	float mag = sqrt(vertexPosition.x * vertexPosition.x + vertexPosition.y * vertexPosition.y);

	if(mag > 1.0){
		discard;
	}

	gl_FragColor = vec4(color.x, color.y, color.z, alpha);

}
