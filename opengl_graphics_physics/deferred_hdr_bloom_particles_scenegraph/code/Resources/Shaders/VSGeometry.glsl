#version 420 core

layout(location = 0) in vec3 Position;
layout(location = 1) in vec2 TexCoord;
layout(location = 2) in vec3 Normal;

layout(std140, binding = 0) uniform GBVars
{
	mat4 MVP;					//16		0
								//16		16
								//16		32
								//16		48
	mat4 M;						//16		64
								//16		80
								//16		96
								//16		112
	vec4 MaterialProperties;	//16		0
	vec3 MaterialColor;			//16		16
	vec2 tiling;				//8			32
	uint objectID;				//4			40 44
};

out vec2 TexCoord0;
out vec3 Normal0;
out vec3 WorldPos0;

void main()
{
	gl_Position = MVP * vec4(Position, 1.0);
	TexCoord0 = TexCoord * tiling;
	Normal0 = (M * vec4(Normal, 0.0)).xyz;
	WorldPos0 = (M * vec4(Position, 1.0)).xyz;
}