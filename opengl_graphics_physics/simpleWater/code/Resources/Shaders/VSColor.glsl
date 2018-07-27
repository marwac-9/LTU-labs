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
out vec3 LightDirection_worldspace;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;
uniform mat4 M;

float amplitude = 1.f;
float speed = 3.f;
float periodicScale = 200.f;

uniform vec3 CameraPos;
uniform vec3 LightInvDirection_worldspace;
uniform vec2 tiling;

uniform vec4 plane;
void main(){

	// Output position of the vertex, in clip space : MVP * position
	gl_Position = MVP * vec4(vertexPosition_modelspace, 1);
	
	
	// Position of the vertex, in worldspace : M * position
	vec4 Position_world4 = (M * vec4(vertexPosition_modelspace, 1));
	Position_worldspace = Position_world4.xyz;

	gl_ClipDistance[0] = dot(Position_world4, plane);
	// Normal of the the vertex, in camera space
	Normal_worldspace = (M * vec4(vertexNormal_modelspace, 0)).xyz; // Only correct if ModelMatrix does not scale the model ! Use its inverse transpose if not.
	
	// Vector that goes from the vertex to the camera, in world space.
	EyeDirection_worldspace = CameraPos - Position_worldspace;
	
	LightDirection_worldspace = LightInvDirection_worldspace;

	UV = vertexUV*tiling;
}