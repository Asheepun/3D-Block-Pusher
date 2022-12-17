#version 330 core

uniform float alpha;

uniform vec3 color;

void main(){

	gl_FragColor = vec4(color.x, color.y, color.z, alpha);

	gl_FragDepth = 0.0;

}
