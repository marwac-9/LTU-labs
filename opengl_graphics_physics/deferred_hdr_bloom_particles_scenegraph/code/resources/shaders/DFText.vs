#version 330

uniform mat4 ciModelViewProjection;

in vec4 ciPosition;
in vec2 ciTexCoord0;
in vec4 ciColor;

out vec2 vTexCoord0;
out vec4 vColor;

void main()
{
	vColor = ciColor;
	vTexCoord0 = ciTexCoord0;

	gl_Position = ciModelViewProjection * ciPosition;
}