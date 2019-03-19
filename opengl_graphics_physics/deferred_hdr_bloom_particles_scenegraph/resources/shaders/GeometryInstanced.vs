#version 420 core

layout(location = 0) in vec3 Position;
layout(location = 1) in vec2 TexCoord;
layout(location = 2) in vec3 Normal;
layout(location = 3) in mat4 M;
layout(location = 7) in uint objectID;
layout(location = 8) in vec3 MaterialColor;
layout(location = 9) in vec4 MaterialProperties;

out vec2 TexCoord0;
out vec3 Normal0;
out vec3 WorldPos0;
flat out uint objectID0;
out vec3 MaterialColor0;
out vec4 MaterialProperties0;

uniform mat4 VP;
uniform vec2 tiling;

void main()
{
	gl_Position = VP * M * vec4(Position, 1.0);
	TexCoord0 = TexCoord * tiling;
	Normal0 = (M * vec4(Normal, 0.0)).xyz;
	WorldPos0 = (M * vec4(Position, 1.0)).xyz;
	objectID0 = objectID;
	MaterialColor0 = MaterialColor;
	MaterialProperties0 = MaterialProperties;
}