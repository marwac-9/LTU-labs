#version 330

uniform vec3 MaterialColorValue;

layout(location = 0) out vec3 DiffuseOut;
layout(location = 1) out vec4 diffIntAmbIntShineSpecIntOut;

void main()
{
	DiffuseOut = MaterialColorValue;
	diffIntAmbIntShineSpecIntOut = vec4(1.f, 1.f, 1.f, 1.f);
} 
