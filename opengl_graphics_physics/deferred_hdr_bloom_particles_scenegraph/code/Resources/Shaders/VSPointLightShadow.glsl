#version 420 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;

layout(std140, binding = 1) uniform LBVars
{
	vec3 lightInvDir;				//0
	float shadowTransitionSize;		//12
	float outerCutOff;				//16
	float innerCutOff;				//20
	float lightRadius;				//24
	float lightPower;				//28
	vec3 lightColor;				//32
	float ambient;					//44
	mat4 depthBiasMVP;				//48
	mat4 MVP;						//112
	vec3 lightPosition;				//176
	float constant;					//188
	float linear;					//192
	float exponential;				//196 -> 200
};

void main(){
	gl_Position = MVP * vec4(vertexPosition_modelspace, 1);
}