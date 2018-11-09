#version 330 core

in vec3 UVdirection;

layout(location = 0) out vec4 color;
layout(location = 1) out vec4 brightColor;

uniform samplerCube cubeMap;
uniform vec3 MaterialColor;

void main(){
	color = vec4(MaterialColor + texture(cubeMap, UVdirection).xyz, 1.0);
	float brightness = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));
	if (brightness > 1.0)
		brightColor = color;
	else
		brightColor = vec4(0.0, 0.0, 0.0, 1.0);
}