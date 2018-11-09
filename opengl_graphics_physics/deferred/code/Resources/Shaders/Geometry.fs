#version 420

in vec2 TexCoord0;
in vec3 Normal0;
in vec3 WorldPos0;

layout(location = 0) out vec3 WorldPosOut;
layout(location = 1) out vec3 DiffuseOut;
layout(location = 2) out vec3 NormalOut;

layout(location = 3) out vec4 metDiffIntShineSpecIntOut;

layout(std140, binding = 0) uniform GBVars
{
	mat4 MVP;
	mat4 M;
	vec4 MaterialProperties;
	vec3 MaterialColor;
	vec2 tiling;
	uint objectID;
};

uniform sampler2D myTextureSampler;

void main()
{
	WorldPosOut = WorldPos0;
	DiffuseOut = MaterialColor + texture(myTextureSampler, TexCoord0).xyz;
	NormalOut = normalize(Normal0);

	metDiffIntShineSpecIntOut = MaterialProperties;
}