#version 330

uniform vec4 gObjectIndexVec4;

out vec4 FragColor;

void main()
{
    FragColor = gObjectIndexVec4;
} 
