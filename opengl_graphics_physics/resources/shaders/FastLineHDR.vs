#version 420 core

layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec4 color;

out vec4 lineColor;

layout(std140, binding = 2) uniform CBVars
{
	mat4 VP;
	vec2 screenSize;
	float near;
	float far;
	vec3 cameraPos;
	vec3 cameraUp;
	vec3 cameraRight;
	vec3 cameraForward;
};

void main()
{
	gl_Position = VP*vec4(vertexPos, 1.0f);

	lineColor = color;
}

