#version 330 core

in vec2 UV;

// Ouput data
layout(location = 0) out vec3 color;

layout(binding = 0) uniform sampler2D shadowMapSampler;

void main(){
	//Render Shadowmap texture to fullscreen quad
	//float depthSample = pow(texture2D(shadowMapSampler, UV).x, 10);
	color = texture2D(shadowMapSampler, UV).xyz;
}