#version 330

uniform vec3 MaterialColorValue;

out vec3 color;

void main()
{
    color = MaterialColorValue;
} 
