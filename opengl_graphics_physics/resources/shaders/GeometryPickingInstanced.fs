#version 420 core

in vec2 TexCoord0;
in mat3 TBN0;
in vec3 WorldPos0;
flat in uint objectID0;
in vec4 MaterialColorShininess0;

layout(location = 0) out vec3 WorldPosOut;
layout(location = 1) out vec3 DiffuseOut;
layout(location = 2) out vec3 NormalOut;

layout(location = 3) out vec4 SpecularOrAoRoughnessMetallicOut;
layout(location = 4) out uint PickingOut;

layout(binding = 0) uniform sampler2D myTextureSampler;
layout(binding = 1) uniform sampler2D normalSampler;
layout(binding = 2) uniform sampler2D specularOraoRoughnessMetallicSampler;

void main()
{
	WorldPosOut = WorldPos0;
	DiffuseOut = MaterialColorShininess0.xyz + texture(myTextureSampler, TexCoord0).xyz;

	vec3 normalMapNormal = 2 * texture(normalSampler, TexCoord0).xyz - 1.0;
	normalMapNormal = normalize(normalMapNormal);
	NormalOut = normalize(TBN0 * normalMapNormal);

	float Shininess = MaterialColorShininess0.w;
	SpecularOrAoRoughnessMetallicOut.xyz = texture(specularOraoRoughnessMetallicSampler, TexCoord0).xyz;
	SpecularOrAoRoughnessMetallicOut.w = Shininess;
	PickingOut = objectID0;
}