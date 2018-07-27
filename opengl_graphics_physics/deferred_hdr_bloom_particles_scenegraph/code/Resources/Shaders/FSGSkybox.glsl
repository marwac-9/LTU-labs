#version 330 core

in vec3 UVdirection;
in vec3 Normal0;

layout(location = 0) out vec4 DiffuseOut;

uniform vec3 MaterialColor;
uniform samplerCube cubeMap;

void main(){
	DiffuseOut = vec4(MaterialColor + texture(cubeMap, UVdirection).xyz, 1.0);
}