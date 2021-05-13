#version 420 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal_modelspace;

// Output data ; will be interpolated for each fragment.
out vec2 UV;
out vec3 Position_worldspace;
out vec3 Normal_worldspace;
out vec3 EyeDirection_worldspace;
out vec3 LightDirection_worldspace;

layout(std140, binding = 2) uniform CBVars
{
	mat4 VP;
	vec2 screenSize;
	float near;
	float far;
	vec3 cameraPos;
	vec3 cameraUp;
	vec3 cameraRight;
	vec3 cameraForward;
};

uniform mat4 M;

uniform vec3 LightInvDirection_worldspace;
uniform vec2 tiling;

uniform vec4 plane;
void main(){

	gl_Position = VP * M * vec4(vertexPosition_modelspace, 1);
	vec4 Position_world4 = (M * vec4(vertexPosition_modelspace, 1));
	Position_worldspace = Position_world4.xyz;

	gl_ClipDistance[0] = dot(Position_world4, plane);
	Normal_worldspace = (M * vec4(vertexNormal_modelspace, 0)).xyz;
	
	EyeDirection_worldspace = cameraPos - Position_worldspace;
	
	LightDirection_worldspace = LightInvDirection_worldspace;

	UV = vertexUV*tiling;
}