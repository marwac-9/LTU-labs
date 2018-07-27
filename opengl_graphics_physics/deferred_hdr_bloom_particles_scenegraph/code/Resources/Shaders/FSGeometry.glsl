#version 330 core

in vec2 TexCoord0;
in vec3 Normal0;
in vec3 WorldPos0;

layout(location = 0) out vec3 WorldPosOut;
layout(location = 1) out vec3 DiffuseOut;
layout(location = 2) out vec3 NormalOut;

layout(location = 3) out vec4 metDiffIntShineSpecIntOut;
layout(location = 4) out uint PickingOut;

uniform vec4 MaterialProperties;
uniform vec3 MaterialColor;
uniform sampler2D myTextureSampler;
uniform uint objectID;

void main()
{
	WorldPosOut = WorldPos0;
	DiffuseOut = MaterialColor + texture(myTextureSampler, TexCoord0).xyz;
	NormalOut = normalize(Normal0);

	metDiffIntShineSpecIntOut = MaterialProperties;
	PickingOut = objectID;
}