#version 330 core
in vec4 fragmentPosition;
in vec2 texturePosition;
in vec4 fragmentNormal;
in mat4 modelMatrix;
in mat4 modelRotationMatrix;
in vec4 inputColor;

out vec4 FragColor;

uniform sampler2D colorTexture;
uniform sampler2D shadowMapDepthTexture;
uniform sampler2D transparentShadowMapDepthTexture;
uniform sampler2D transparentShadowMapColorTexture;

uniform mat4 cameraMatrix;
uniform mat4 perspectiveMatrix;
uniform mat4 lightCameraMatrix;

uniform float shadowMapScale;

//uniform vec4 inputColor;
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

	//get color value from texture and combine with input color
    vec4 color = texture2D(colorTexture, vec2(texturePosition.x, 1.0 - texturePosition.y));

	color.x *= inputColor.x;
	color.y *= inputColor.y;
	color.z *= inputColor.z;
	color.w *= inputColor.w;

	float alpha = color.w;

	//calculate diffuse light
	float diffuseLight = abs(dot(lightDirection, modelNormal.xyz));

	//calculate specular light
	vec3 viewDirection = -normalize(relativeModelPosition.xyz);
	vec3 viewLightHalfway = normalize((viewDirection + normalize(-lightDirection)));

	float specularLight = max(dot(modelNormal.xyz, viewLightHalfway), 0.0);

	//check if fragment is in shadow and apply lights accordingly
	float fragmentShadowDepth = lightRelativeModelPosition.z / 100.0;

	vec2 shadowMapTexturePosition = (vec2(1.0, 1.0) + ((lightRelativeModelPosition.xy) / shadowMapScale)) / 2.0;
	float shadowMapDepth = texture2D(shadowMapDepthTexture, shadowMapTexturePosition).r;
	float transparentShadowMapDepth = texture2D(transparentShadowMapDepthTexture, shadowMapTexturePosition).r;
	vec4 transparentShadowMapColor = texture2D(transparentShadowMapColorTexture, shadowMapTexturePosition);

	float shadowDepthTolerance = 0.0005;
	float shadowDepthDiff = fragmentShadowDepth - shadowMapDepth;
	float transparentShadowDepthDiff = fragmentShadowDepth - transparentShadowMapDepth;

	bool inShadow = dot(lightDirection, modelNormal.xyz) > 0 || shadowDepthDiff > shadowDepthTolerance;
	bool inTransparentShadow = transparentShadowDepthDiff > shadowDepthTolerance;

	//inShadow = false;
	//inTransparentShadow = false;

	if(inShadow){
		color *= ambientLightFactor;
	}else if(inTransparentShadow){
		color *= ambientLightFactor + transparentShadowMapColor * lightColor * (diffuseLight * diffuseLightFactor + pow(specularLight, shinyFactor) * specularLightFactor) * (1.0 - transparentShadowMapColor.w);
	}else{
		color *= (ambientLightFactor + diffuseLightFactor * diffuseLight + specularLightFactor * pow(specularLight, shinyFactor));
	}
	color *= lightColor;

	//do gamma calculations
	color.x = pow(color.x, gammaFactor);
	color.y = pow(color.y, gammaFactor);
	color.z = pow(color.z, gammaFactor);

	//set final frag color
	FragColor = vec4(color.xyz, alpha);

	//handle fragment depth
	float depth = relativeModelPosition.z / 100.0;

	if(alpha < 1.0){
		depth -= 0.00001;
	}

	gl_FragDepth = depth;

} 
