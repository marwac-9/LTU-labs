#version 330

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec3 worldPosition;

uniform vec4 gObjectIndexVec4;

in vec3 worldPos;

void main()
{
    FragColor = gObjectIndexVec4;
    worldPosition = worldPos;
} 
