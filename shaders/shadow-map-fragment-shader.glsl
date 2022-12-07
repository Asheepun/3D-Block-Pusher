#version 330 core
in vec4 fragmentPosition;

out vec4 FragColor;

uniform mat4 modelMatrix;
uniform mat4 modelRotationMatrix;
uniform mat4 cameraMatrix;

void main(){

	vec4 shadowMapModelPosition = fragmentPosition * modelRotationMatrix * modelMatrix * cameraMatrix;

	float depth = shadowMapModelPosition.z / 100.0;

	gl_FragColor = vec4(depth, depth, depth, 1.0);

	gl_FragDepth = depth;

} 
