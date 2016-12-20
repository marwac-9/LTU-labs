#version 330 core

in vec4 bbcolor;

// Ouput data
layout(location = 0) out vec4 color;
layout(location = 1) out vec4 brightColor;

void main(){
	// Output color = color of the texture at the specified UV
	color = bbcolor;
	float brightness = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));
	if (brightness > 1.0)
		brightColor = color;
	else
		brightColor = vec4(0.0, 0.0, 0.0, 1.0);
}