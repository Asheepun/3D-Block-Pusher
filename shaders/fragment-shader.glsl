#version 330 core
in vec4 fragmentPosition;
in vec2 texturePosition;
in vec4 fragmentNormal;

out vec4 FragColor;

uniform sampler2D colorTexture;
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

float lightStrength = 1.0;

float ambientLightFactor = 0.3;
float diffuseLightFactor = 0.7;

void main(){

	FragColor = vec4(1.0, 0.0, 1.0, 1.0);

	vec4 modelNormal = fragmentNormal * modelRotationMatrix;
	vec4 modelPosition = fragmentPosition * modelRotationMatrix * modelMatrix;
	vec4 relativeModelPosition = modelPosition * cameraMatrix;

	vec4 lightRelativeModelPosition = (modelPosition) * lightCameraMatrix;

    vec4 color = texture2D(colorTexture, vec2(texturePosition.x, 1.0 - texturePosition.y));

	color.x *= inputColor.x;
	color.y *= inputColor.y;
	color.z *= inputColor.z;
	color.w *= inputColor.w;

	float alpha = color.w;

	float diffuseLight = abs(dot(lightDirection, modelNormal.xyz));

	float fragmentShadowDepth = lightRelativeModelPosition.z / 100.0;

	float shadowDepthBias = 0.003;
	float shadowDepthMaxBias = 0.006;

	shadowDepthBias = 0.0;
	shadowDepthMaxBias = 0.0;

	vec2 shadowMapTexturePosition = (vec2(1.0, 1.0) + ((lightRelativeModelPosition.xy) / shadowMapScale)) / 2.0;
	float shadowMapDepth = texture2D(shadowMapDepthTexture, shadowMapTexturePosition).r;
	float transparentShadowMapDepth = texture2D(transparentShadowMapDepthTexture, shadowMapTexturePosition).r;
	vec4 transparentShadowMapColor = texture2D(transparentShadowMapColorTexture, shadowMapTexturePosition);

	float shadowDepthTolerance = 0.0005;
	float shadowDepthDiff = fragmentShadowDepth - shadowMapDepth;
	float transparentShadowDepthDiff = fragmentShadowDepth - transparentShadowMapDepth;
	//bool inShadow = dot(lightDirection, modelNormal.xyz) > 0 || fragmentShadowDepth > shadowMapDepth + max(shadowDepthBias * (1 - dot(lightDirection, modelNormal.xyz)), shadowDepthMaxBias);

	bool inShadow = dot(lightDirection, modelNormal.xyz) > 0 || shadowDepthDiff > shadowDepthTolerance;
	bool inTransparentShadow = transparentShadowDepthDiff > shadowDepthTolerance;

	vec4 shadowedColor = color * (ambientLightFactor + diffuseLightFactor * diffuseLight);

	if(inTransparentShadow){
		//shadowedColor = vec4(1.0, 0.0, 0.0, 1.0);
		shadowedColor = color * ambientLightFactor + transparentShadowMapColor * diffuseLight * diffuseLightFactor;
		//color += diffuseLight * diffuseLightFactor * shadowMapColor;
	}

	if(inShadow){
		shadowedColor = color * ambientLightFactor;
	}

	color = shadowedColor;

	//if(inShadow && shadowMapColor.w < 1.0){
		//color = shadowMapColor * shadowMapColor.w + color * (1.0 - shadowMapColor.w);
		//color = shadowMapColor * shadowMapColor.w + color * (1.0 - shadowMapColor.w);
		//color += diffuseLight * diffuseLightFactor * shadowMapColor;
	//}


	//if(shadowMapColor.w < 1.0){
		//color = vec4(1.0, 0.0, 0.0, 1.0);
	//}

	//color.x = 0.0;
	//color.y = 0.0;
	//color.z = 0.0;
	//color.x = (1.0 + modelNormal.x) / 2.0;
	//color.y = (1.0 + modelNormal.y) / 2.0;
	//color.z = (1.0 + modelNormal.z) / 2.0;

	FragColor = vec4(color.xyz, alpha);

	float depth = relativeModelPosition.z / 100.0;

	if(alpha < 1.0){
		depth -= 0.00001;
	}

	gl_FragDepth = depth;

} 
