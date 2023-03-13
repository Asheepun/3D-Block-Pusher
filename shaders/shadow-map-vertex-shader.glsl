#version 330 core
layout (location = 0) in vec3 attribute_vertex;
layout (location = 1) in vec2 attribute_textureVertex;
layout (location = 2) in vec3 attribute_normalVertex;

//out vec4 fragmentPosition;
//out vec4 relativeModelPosition;
out float depth;
out vec4 inputColor;

uniform samplerBuffer modelMatrixTextureBuffer;
uniform samplerBuffer modelRotationMatrixTextureBuffer;
uniform samplerBuffer inputColorTextureBuffer;

//uniform mat4 modelMatrix;
//uniform mat4 modelRotationMatrix;
uniform mat4 cameraMatrix;
uniform vec3 lightDirection;

uniform float shadowMapScale;

void main(){

	//get instanced variables from texture buffers
	mat4 modelMatrix = mat4(
		texelFetch(modelMatrixTextureBuffer, gl_InstanceID * 4 + 0),
		texelFetch(modelMatrixTextureBuffer, gl_InstanceID * 4 + 1),
		texelFetch(modelMatrixTextureBuffer, gl_InstanceID * 4 + 2),
		texelFetch(modelMatrixTextureBuffer, gl_InstanceID * 4 + 3)
	);

	mat4 modelRotationMatrix = mat4(
		texelFetch(modelRotationMatrixTextureBuffer, gl_InstanceID * 4 + 0),
		texelFetch(modelRotationMatrixTextureBuffer, gl_InstanceID * 4 + 1),
		texelFetch(modelRotationMatrixTextureBuffer, gl_InstanceID * 4 + 2),
		texelFetch(modelRotationMatrixTextureBuffer, gl_InstanceID * 4 + 3)
	);

	inputColor = texelFetch(inputColorTextureBuffer, gl_InstanceID);

	//set positions
	vec4 vertexPosition = vec4(attribute_vertex.xyz, 1.0);

	vec4 relativeModelPosition = vertexPosition * modelRotationMatrix * modelMatrix * cameraMatrix;

	depth = relativeModelPosition.z / 100.0;

	relativeModelPosition.z = 0.0;
	relativeModelPosition.w = shadowMapScale;

	gl_Position = relativeModelPosition;

}

