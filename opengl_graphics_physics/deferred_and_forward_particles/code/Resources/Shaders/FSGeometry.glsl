#version 330 core

in vec2 TexCoord0;
in vec3 Normal0;
in vec3 WorldPos0;

in vec4 diffIntAmbIntShineSpecInt0;
in vec3 materialColor0;

layout(location = 0) out vec3 WorldPosOut;
layout(location = 1) out vec3 DiffuseOut;
layout(location = 2) out vec3 NormalOut;

layout(location = 3) out vec4 diffIntAmbIntShineSpecIntOut;

uniform sampler2D myTextureSampler;

void main()
{
	WorldPosOut = WorldPos0;
	DiffuseOut = materialColor0 + texture(myTextureSampler, TexCoord0).xyz;
	NormalOut = normalize(Normal0);

	diffIntAmbIntShineSpecIntOut = diffIntAmbIntShineSpecInt0;
}