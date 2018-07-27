#version 330 core

layout(location = 0) in vec3 Position;
layout(location = 1) in vec2 TexCoord;
layout(location = 2) in vec3 Normal;

uniform mat4 MVP;
uniform mat4 M;

uniform vec4 MaterialProperties;
uniform vec3 MaterialColor;
uniform vec2 tiling;

out vec2 TexCoord0;
out vec3 Normal0;
out vec3 WorldPos0;

out vec3 materialColor0;
out vec4 metDiffIntShineSpecInt0;

void main()
{
	gl_Position = MVP * vec4(Position, 1.0);
	TexCoord0 = TexCoord * tiling;
	Normal0 = (M * vec4(Normal, 0.0)).xyz;
	WorldPos0 = (M * vec4(Position, 1.0)).xyz;

	materialColor0 = MaterialColor;
	metDiffIntShineSpecInt0 = MaterialProperties;
}