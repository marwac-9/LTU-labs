#version 420

layout (location = 0) in vec3 Position;

layout(std140, binding = 0) uniform GBVars
{
	mat4 M;
	vec4 MaterialColorShininess;
	vec2 tiling;
	uint objectID;
};

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

out vec3 worldPos;

void main()
{
    gl_Position = VP * M * vec4(Position, 1.0);
    worldPos = (M * vec4(Position, 1.0)).xyz;
} 
