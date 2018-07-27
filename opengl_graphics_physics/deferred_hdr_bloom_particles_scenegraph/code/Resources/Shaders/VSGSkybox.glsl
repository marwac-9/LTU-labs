#version 330 core

layout(location = 0) in vec3 Position;

uniform mat4 MVP;

out vec3 UVdirection;

void main(){
	gl_Position = (MVP * vec4(Position, 1)).xyww;
	UVdirection = Position;
}