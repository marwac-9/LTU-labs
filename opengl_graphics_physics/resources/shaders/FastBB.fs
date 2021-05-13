#version 420 core

in vec3 bbcolor;

// Ouput data
layout(location = 0) out vec4 color;

void main(){
	// Output color = color of the texture at the specified UV
	color = vec4(bbcolor,1.0);
}