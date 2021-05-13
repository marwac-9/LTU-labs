#version 420 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 squareVertices;
layout(location = 1) in vec4 xyzs; // Position of the center of the particule and size of the square
layout(location = 2) in vec4 color; // Position of the center of the particule and size of the square

// Output data ; will be interpolated for each fragment.
out vec2 UV;
out vec4 particlecolor;

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

void main()
{
	float particleSize = xyzs.w; // because we encoded it this way.
	vec3 particleCenter = xyzs.xyz;
	
	vec3 vertexPosition = 
		particleCenter
		+ cameraRight * squareVertices.x * particleSize
		+ cameraUp * squareVertices.y * particleSize;

	// Output position of the vertex
	gl_Position = VP*vec4(vertexPosition, 1.0f);

	// UV of the vertex. No special space for this one.
	UV = squareVertices.xy + vec2(0.5, 0.5);
	particlecolor = color;
}

