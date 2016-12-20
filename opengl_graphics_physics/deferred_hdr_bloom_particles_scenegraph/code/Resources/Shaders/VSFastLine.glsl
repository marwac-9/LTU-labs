#version 330 core

layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec4 color;

out vec4 lineColor;

uniform mat4 VP; 

void main()
{
	gl_Position = VP*vec4(vertexPos, 1.0f);

	lineColor = color;
}

