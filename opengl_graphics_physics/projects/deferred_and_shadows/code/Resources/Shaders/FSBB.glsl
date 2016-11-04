#version 330

uniform vec3 MaterialColorValue;

layout(location = 0) out vec3 DiffuseOut;
layout(location = 1) out vec3 diffIntAmbIntShineOut;

void main()
{
	DiffuseOut = MaterialColorValue;
	diffIntAmbIntShineOut = vec3(1.f, 1.f, 1.f);
} 
