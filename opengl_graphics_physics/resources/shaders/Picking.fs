#version 420

layout (location = 0) out uint FragColor;

layout(std140, binding = 0) uniform GBVars
{
	mat4 M;
	vec4 MaterialColorShininess;
	vec2 tiling;
	uint objectID;
};

void main()
{
	FragColor = objectID;
} 
