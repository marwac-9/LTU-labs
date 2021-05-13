#version 420 core

// Interpolated values from the vertex shaders
in vec2 UV;
in vec4 particlecolor;

// Ouput data
out vec4 color;

layout(binding = 0) uniform sampler2D myTextureSampler;

void main(){
	// Output color = color of the texture at the specified UV
	color = texture( myTextureSampler, UV ) * particlecolor;

}