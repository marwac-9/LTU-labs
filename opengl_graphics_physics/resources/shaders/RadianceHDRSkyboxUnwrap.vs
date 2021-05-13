#version 420 core

layout(location = 0) in vec3 Position;

uniform mat4 MVPSkybox;

out vec3 UVdirection;

void main(){
	gl_Position = (MVPSkybox * vec4(Position, 1));
	UVdirection = vec3(Position.x, -Position.y, Position.z);
}
