#version 420

layout (location = 0) in vec3 Position;

layout(std140, binding = 0) uniform GBVars
{
	mat4 MVP;
	mat4 M;
	vec4 MaterialColorShininess;
	vec2 tiling;
	uint objectID;
};

out vec3 worldPos;

void main()
{
    gl_Position = MVP * vec4(Position, 1.0);
    worldPos = (M * vec4(Position, 1.0)).xyz;
} 
