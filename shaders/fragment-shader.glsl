#version 330 core
in vec4 fragmentPosition;
in vec2 texturePosition;
in vec4 fragmentNormal;

out vec4 FragColor;

uniform sampler2D colorTexture;
uniform sampler2D shadowMapTexture;

uniform mat4 modelMatrix;
uniform mat4 modelRotationMatrix;
uniform mat4 cameraMatrix;
uniform mat4 perspectiveMatrix;
uniform mat4 lightCameraMatrix;

uniform float shadowMapScale;

uniform vec4 color;
uniform vec3 lightDirection;

float lightStrength = 1.0;

float ambientLightFactor = 0.1;
float diffuseLightFactor = 0.9;

void main(){

	vec4 modelNormal = normalize(fragmentNormal * modelRotationMatrix);
	vec4 modelPosition = fragmentPosition * modelRotationMatrix * modelMatrix;
	vec4 relativeModelPosition = modelPosition * cameraMatrix;

	float shadowDepthNormalBias = -0.1;

	vec4 lightRelativeModelPosition = (modelPosition) * lightCameraMatrix;

    vec4 color = texture2D(colorTexture, vec2(texturePosition.x, 1.0 - texturePosition.y));
    //vec4 color = vec4(1.0, 1.0, 1.0, 1.0);

	float alpha = color.w;

	float diffuseLight = abs(dot(lightDirection, modelNormal.xyz));

	float fragmentShadowDepth = lightRelativeModelPosition.z / 100.0;

	float shadowDepthBias = 0.0;

	float shadowMapDepth = texture2D(shadowMapTexture, (vec2(1.0, 1.0) + (lightRelativeModelPosition.xy / shadowMapScale)) / 2.0).r;

	//is set to zero when statment is false
	diffuseLight *= float(!(dot(lightDirection, modelNormal.xyz) > 0 || fragmentShadowDepth > shadowMapDepth + shadowDepthBias));

	//if(dot(lightDirection, modelNormal.xyz) > 0
	//|| fragmentShadowDepth > shadowMapDepth + shadowDepthBias){
		//diffuseLight = 0.0;
	//}

	color *= ambientLightFactor + diffuseLightFactor * diffuseLight;

	FragColor = vec4(color.xyz, alpha);

	float depth = relativeModelPosition.z / 100.0;

	gl_FragDepth = depth;

} 
