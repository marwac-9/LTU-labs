#version 420 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;

layout(std140, binding = 1) uniform LBVars
{
	vec3 lightInvDir;
	float shadowTransitionSize;
	float outerCutOff;
	float innerCutOff;
	float lightRadius;
	float lightPower;
	vec3 lightColor;
	float ambient;
	mat4 depthBiasMVP;
	mat4 MVP;
	vec3 lightPosition;
	float constant;
	float linear;
	float exponential;
};

void main(){
	gl_Position = MVP * vec4(vertexPosition_modelspace, 1);
}