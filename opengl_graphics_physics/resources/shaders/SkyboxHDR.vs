#version 420 core

layout(location = 0) in vec3 Position;

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

out vec3 UVdirection;

void main(){
	mat4 zeroedRow3VP = VP;
	zeroedRow3VP[3][0] = 0;
	zeroedRow3VP[3][1] = 0;
	zeroedRow3VP[3][2] = 0;
	zeroedRow3VP[3][3] = 0;
	gl_Position = (zeroedRow3VP * vec4(Position, 1)).xyww;
	UVdirection = Position;
}