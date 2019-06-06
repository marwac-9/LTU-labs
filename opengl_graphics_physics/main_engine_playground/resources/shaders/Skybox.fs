#version 420 core

in vec3 UVdirection;

layout(location = 0) out vec4 color;
layout(location = 1) out vec4 brightColor;

layout(binding = 0) uniform samplerCube cubeMap;

void main(){
	color = vec4(texture(cubeMap, UVdirection).xyz, 1.0);
	float brightness = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));
	if (brightness > 1.0)
		brightColor = color;
	else
		brightColor = vec4(0.0, 0.0, 0.0, 1.0);
}