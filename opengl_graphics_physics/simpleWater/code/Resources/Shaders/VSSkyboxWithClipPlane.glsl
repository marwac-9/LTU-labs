#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;

out vec3 UVdirection;
uniform mat4 MVP;
uniform vec4 plane;
void main(){

	gl_Position = MVP * vec4(vertexPosition_modelspace, 1);
	UVdirection = vertexPosition_modelspace;
	gl_ClipDistance[0] = dot(vec4(vertexPosition_modelspace,1), plane);
}