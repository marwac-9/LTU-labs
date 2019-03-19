#version 420 core

in vec3 UVdirection;
in vec3 Normal0;

layout(location = 1) out vec4 DiffuseOut;

uniform vec3 MaterialColor;
layout(binding = 0) uniform samplerCube cubeMap;

void main(){
	DiffuseOut = vec4(MaterialColor + texture(cubeMap, UVdirection).xyz, 1.0);
}