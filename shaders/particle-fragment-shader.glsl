#version 330 core
in vec4 fragmentPosition;
in vec2 texturePosition;
in vec4 fragmentNormal;

out vec4 FragColor;

uniform sampler2D shadowMapDepthTexture;
uniform sampler2D transparentShadowMapDepthTexture;
uniform sampler2D transparentShadowMapColorTexture;

uniform mat4 modelMatrix;
uniform mat4 modelRotationMatrix;
uniform mat4 cameraMatrix;
uniform mat4 perspectiveMatrix;
uniform mat4 lightCameraMatrix;

uniform float shadowMapScale;

uniform vec4 inputColor;
uniform vec3 lightDirection;

vec4 lightColor = vec4(1.0, 1.0, 0.9, 1.0);

float lightStrength = 1.0;

float ambientLightFactor = 0.2;
float diffuseLightFactor = 0.7;
float specularLightFactor = 0.5;

float gammaFactor = 1.3;

float shinyFactor = 1.0;

void main(){

	//calculate positions
	vec4 modelNormal = fragmentNormal * modelRotationMatrix;
	vec4 modelPosition = fragmentPosition * modelRotationMatrix * modelMatrix;
	vec4 relativeModelPosition = modelPosition * cameraMatrix;
	vec4 lightRelativeModelPosition = modelPosition * lightCameraMatrix;

	float alpha = inputColor.w;

	FragColor = vec4(inputColor.xyz, alpha);

	//handle fragment depth
	float depth = relativeModelPosition.z / 100.0;

	if(alpha < 1.0){
		depth -= 0.00001;
	}

	gl_FragDepth = depth;

} 

