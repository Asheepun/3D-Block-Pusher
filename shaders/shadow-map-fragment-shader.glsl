#version 330 core
//in vec4 fragmentPosition;
//in vec4 relativeModelPosition;
in float depth;
in vec4 inputColor;

uniform mat4 modelMatrix;
uniform mat4 modelRotationMatrix;
uniform mat4 cameraMatrix;
//uniform vec4 inputColor;

void main(){

	//vec4 shadowMapModelPosition = fragmentPosition * modelRotationMatrix * modelMatrix * cameraMatrix;

	//float depth = shadowMapModelPosition.z / 100.0;
	//float depth = relativeModelPosition.z / 100.0;

	//gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
	gl_FragColor = inputColor;

	gl_FragDepth = depth;

} 
