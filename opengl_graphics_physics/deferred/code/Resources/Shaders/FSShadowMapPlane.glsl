#version 330 core

in vec2 UV;

// Ouput data
layout(location = 0) out vec3 color;

uniform sampler2D shadowMapSampler;

void main(){
	//Render Shadowmap texture to fullscreen quad
	color = texture2D(shadowMapSampler, UV).xyz;
}