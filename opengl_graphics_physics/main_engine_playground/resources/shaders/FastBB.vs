#version 330 core

layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec3 color;
layout(location = 2) in mat4 model;

out vec3 bbcolor;

uniform mat4 VP; 

void main()
{
	gl_Position = VP*model*vec4(vertexPos, 1.0f);

	bbcolor = color;
}

