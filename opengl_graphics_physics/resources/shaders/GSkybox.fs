#version 420 core

in vec3 UVdirection;

layout(location = 1) out vec4 DiffuseOut;

layout(std140, binding = 1) uniform LBVars
{
	mat4 depthBiasMVP;
	vec3 lightInvDir;
	float shadowTransitionSize;
	float outerCutOff;
	float innerCutOff;
	float lightRadius;
	float lightPower;
	vec3 lightColor;
	float ambient;
	float diffuse;
	float specular;
	vec3 lightPosition;
	float constant;
	float linear;
	float exponential;
};

layout(binding = 0) uniform samplerCube cubeMap;

void main(){
	DiffuseOut = vec4(lightColor * lightPower * texture(cubeMap, UVdirection).xyz, 1.0);
}