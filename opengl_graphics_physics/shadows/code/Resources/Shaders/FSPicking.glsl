#version 330

layout (location = 0) out uint FragColor;
layout (location = 1) out vec3 worldPosition;

uniform uint objectID;

in vec3 worldPos;

void main()
{
	FragColor = objectID;
    worldPosition = worldPos;
} 
