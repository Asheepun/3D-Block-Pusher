#version 330 core
layout (location = 0) in vec3 attribute_vertex;
layout (location = 1) in vec2 attribute_textureVertex;
layout (location = 2) in vec3 attribute_normalVertex;

out vec4 fragmentPosition;
out vec2 texturePosition;
out vec4 fragmentNormal;
out mat4 modelMatrix;
out mat4 modelRotationMatrix;
out vec4 inputColor;

uniform samplerBuffer modelMatrixTextureBuffer;
uniform samplerBuffer modelRotationMatrixTextureBuffer;
uniform samplerBuffer inputColorTextureBuffer;
uniform samplerBuffer textureAtlasCoordinatesTextureBuffer;

uniform mat4 cameraMatrix;
uniform mat4 perspectiveMatrix;

void main(){

	//get instanced variables from texture buffers
	modelMatrix = mat4(
		texelFetch(modelMatrixTextureBuffer, gl_InstanceID * 4 + 0),
		texelFetch(modelMatrixTextureBuffer, gl_InstanceID * 4 + 1),
		texelFetch(modelMatrixTextureBuffer, gl_InstanceID * 4 + 2),
		texelFetch(modelMatrixTextureBuffer, gl_InstanceID * 4 + 3)
	);

	modelRotationMatrix = mat4(
		texelFetch(modelRotationMatrixTextureBuffer, gl_InstanceID * 4 + 0),
		texelFetch(modelRotationMatrixTextureBuffer, gl_InstanceID * 4 + 1),
		texelFetch(modelRotationMatrixTextureBuffer, gl_InstanceID * 4 + 2),
		texelFetch(modelRotationMatrixTextureBuffer, gl_InstanceID * 4 + 3)
	);

	inputColor = texelFetch(inputColorTextureBuffer, gl_InstanceID);

	vec4 textureAtlasCoordinates = texelFetch(textureAtlasCoordinatesTextureBuffer, gl_InstanceID);

	//calculate positions
	vec4 vertexPosition = vec4(attribute_vertex.xyz, 1.0);
	vec4 vertexNormal = vec4(attribute_normalVertex.xyz, 1.0);

	//vertexPosition.x += gl_InstanceID * 10;

	//calculate texture position
	texturePosition = attribute_textureVertex;

	//textureAtlasCoordinates = vec4(0.5, 0.0, 0.5, 1.0);

	texturePosition.x *= textureAtlasCoordinates.z;
	texturePosition.y *= textureAtlasCoordinates.w;

	texturePosition.x += textureAtlasCoordinates.x;
	texturePosition.y += textureAtlasCoordinates.y;

	fragmentPosition = vertexPosition;
	fragmentNormal = vertexNormal;

	vec4 projectedPosition = vertexPosition * modelRotationMatrix * modelMatrix * cameraMatrix * perspectiveMatrix;

	gl_Position = projectedPosition;

}
