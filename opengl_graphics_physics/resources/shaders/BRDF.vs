#version 330 core
layout (location = 0) in vec3 vertexPosition_modelspace;

out vec2 TexCoords;

void main()
{
    TexCoords = (vertexPosition_modelspace.xy+vec2(1.0,1.0))/2.0;;
	gl_Position = vec4(vertexPosition_modelspace, 1.0);
}