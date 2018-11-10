#version 330 core

in vec2 UV;

layout(location = 0) out vec3 color;

uniform sampler2D shadowMapSampler;

void main(){
	color = texture2D(shadowMapSampler, UV).xyz;
}