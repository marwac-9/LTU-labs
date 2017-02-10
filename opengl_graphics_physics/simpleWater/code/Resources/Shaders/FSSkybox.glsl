#version 330 core

in vec3 UVdirection;
out vec3 color;

uniform samplerCube cubeMap;

void main(){
	color = texture(cubeMap, UVdirection).rgb;
}