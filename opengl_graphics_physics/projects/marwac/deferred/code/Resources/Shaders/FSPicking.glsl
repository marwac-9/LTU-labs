#version 330

layout (location = 0) out vec4 FragColor;

uniform vec4 gObjectIndexVec4;

void main()
{
    FragColor = gObjectIndexVec4;
} 
