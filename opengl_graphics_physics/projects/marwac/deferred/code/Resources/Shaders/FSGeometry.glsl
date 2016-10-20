#version 330 core

in vec2 TexCoord0;
in vec3 Normal0;
in vec3 WorldPos0;

in vec3 diffIntAmbIntShine0;
in vec3 materialColor0;
in vec3 specularColor0;

layout(location = 0) out vec3 WorldPosOut;
layout(location = 1) out vec3 DiffuseOut;
layout(location = 2) out vec3 NormalOut;
//layout(location = 3) out vec3 TexCoordOut;

layout(location = 3) out vec3 diffIntAmbIntShineOut;
layout(location = 4) out vec3 materialColorOut;
layout(location = 5) out vec3 specularColorOut;

uniform sampler2D myTextureSampler;

void main()
{
	WorldPosOut = WorldPos0;
	DiffuseOut = texture(myTextureSampler, TexCoord0).xyz;
	NormalOut = normalize(Normal0);
	//TexCoordOut = vec3(TexCoord0, 0.0);

	diffIntAmbIntShineOut = diffIntAmbIntShine0;
	materialColorOut = materialColor0;
	specularColorOut = specularColor0;
}