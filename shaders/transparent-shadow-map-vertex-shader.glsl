#version 330 core
layout (location = 0) in vec3 attribute_vertex;
layout (location = 1) in vec2 attribute_textureVertex;
layout (location = 2) in vec3 attribute_normalVertex;

//out vec4 fragmentPosition;
//out vec4 relativeModelPosition;
out float depth;

uniform mat4 modelMatrix;
uniform mat4 modelRotationMatrix;
uniform mat4 cameraMatrix;
uniform vec3 lightDirection;

uniform float shadowMapScale;

void main(){

	vec4 vertexPosition = vec4(attribute_vertex.xyz, 1.0);

	vec4 relativeModelPosition = vertexPosition * modelRotationMatrix * modelMatrix * cameraMatrix;

	depth = relativeModelPosition.z / 100.0;

	relativeModelPosition.z = 0.0;
	relativeModelPosition.w = shadowMapScale;

	gl_Position = relativeModelPosition;

}


