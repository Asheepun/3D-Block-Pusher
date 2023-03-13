#version 330 core
in float depth;
in vec4 inputColor;

uniform mat4 modelMatrix;
uniform mat4 modelRotationMatrix;
uniform mat4 cameraMatrix;

void main(){

	gl_FragColor = inputColor;

	gl_FragDepth = depth;

} 
