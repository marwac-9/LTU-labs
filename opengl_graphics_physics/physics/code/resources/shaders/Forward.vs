#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal_modelspace;

// Output data ; will be interpolated for each fragment.
out vec2 UV;
out vec3 Position_worldspace;
out vec3 Normal_worldspace;
out vec3 EyeDirection_worldspace;
out vec3 PointLightDirectionFromVertex_worldspace;
out vec3 SpotLightDirectionFromVertex_worldspace;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;
uniform mat4 M;
uniform vec3 PointLightPosition_worldspace;
uniform vec3 CameraPos;
uniform vec3 SpotLightPosition_worldspace;

void main(){

	// Output position of the vertex, in clip space : MVP * position
	gl_Position =  MVP * vec4(vertexPosition_modelspace,1);

	Position_worldspace = (M * vec4(vertexPosition_modelspace,1)).xyz;
	
	EyeDirection_worldspace = CameraPos - Position_worldspace;

	PointLightDirectionFromVertex_worldspace = PointLightPosition_worldspace - Position_worldspace;
	
	SpotLightDirectionFromVertex_worldspace = SpotLightPosition_worldspace - Position_worldspace;

	Normal_worldspace = (M * vec4(vertexNormal_modelspace,0)).xyz; // Only correct if ModelMatrix does not scale the model ! Use its inverse transpose if not.

	UV = vertexUV;
}