#version 420

layout (location = 0) out uint oID;
layout (location = 1) out vec3 worldPosition;

layout(std140, binding = 0) uniform GBVars
{
	mat4 M;
	vec4 MaterialColorShininess;
	vec2 tiling;
	uint objectID;
};

in vec3 worldPos;

void main()
{
	oID = objectID;
    worldPosition = worldPos;
} 
