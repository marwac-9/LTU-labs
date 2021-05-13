#version 420 core

in vec3 bbcolor;

// Ouput data
layout(location = 0) out vec4 color;
layout(location = 1) out vec4 brightColor;

void main(){
	// Output color = color of the texture at the specified UV
	color = vec4(bbcolor,1.0);
	float brightness = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));
	if (brightness > 1.0)
		brightColor = color;
	else
		brightColor = vec4(0.0, 0.0, 0.0, 1.0);
}