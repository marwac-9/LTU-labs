#version 420 core

in vec2 TexCoord0;
in vec3 Normal0;
in vec3 WorldPos0;
flat in uint objectID0;
in vec3 MaterialColor0;
in vec4 MaterialProperties0;

layout(location = 0) out vec3 WorldPosOut;
layout(location = 1) out vec3 DiffuseOut;
layout(location = 2) out vec3 NormalOut;

layout(location = 3) out vec4 metDiffIntShineSpecIntOut;
layout(location = 4) out uint PickingOut;

layout(binding = 0) uniform sampler2D myTextureSampler;

void main()
{
	WorldPosOut = WorldPos0;
	DiffuseOut = MaterialColor0 + texture(myTextureSampler, TexCoord0).xyz;
	NormalOut = normalize(Normal0);

	metDiffIntShineSpecIntOut = MaterialProperties0;
	PickingOut = objectID0;
}