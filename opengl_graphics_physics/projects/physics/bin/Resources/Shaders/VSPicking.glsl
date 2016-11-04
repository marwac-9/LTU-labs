#version 330

layout (location = 0) in vec3 Position;

uniform mat4 MVP;
uniform mat4 M;

out vec3 worldPos;

void main()
{
    gl_Position = MVP * vec4(Position, 1.0);
    worldPos = (M * vec4(Position, 1.0)).xyz;
} 
