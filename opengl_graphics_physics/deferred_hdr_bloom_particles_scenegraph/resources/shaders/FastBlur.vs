#version 420 core

layout(location = 0) in vec3 vertexPosition_modelspace;

uniform vec2 offset;

out vec2 blurTextureCoords[11];

void main(){

	gl_Position =  vec4(vertexPosition_modelspace,1);
	vec2 centerTexCoords = vertexPosition_modelspace.xy * 0.5 + 0.5;

	blurTextureCoords[0] = centerTexCoords + offset * -5;
	blurTextureCoords[1] = centerTexCoords + offset * -4;
	blurTextureCoords[2] = centerTexCoords + offset * -3;
	blurTextureCoords[3] = centerTexCoords + offset * -2;
	blurTextureCoords[4] = centerTexCoords + offset * -1;
	blurTextureCoords[5] = centerTexCoords;
	blurTextureCoords[6] = centerTexCoords + offset * 1;
	blurTextureCoords[7] = centerTexCoords + offset * 2;
	blurTextureCoords[8] = centerTexCoords + offset * 3;
	blurTextureCoords[9] = centerTexCoords + offset * 4;
	blurTextureCoords[10] = centerTexCoords + offset * 5;	
}