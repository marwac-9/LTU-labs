#version 420 core

in vec2 UV;

layout(location = 0) out vec3 color;

layout(binding = 0) uniform sampler2D textureSampler;

void main(){
	color = texture2D(textureSampler, UV).xyz;
}