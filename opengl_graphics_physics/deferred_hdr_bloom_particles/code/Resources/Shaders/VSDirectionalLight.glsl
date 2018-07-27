#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;

void main(){
	gl_Position = vec4(vertexPosition_modelspace, 1);
	
	//UV = (vertexPosition_modelspace.xy + vec2(1, 1)) / 2.0;
	// Same, but with the light's view matrix
	//ShadowCoord = DepthBiasMVP * vec4(vertexPosition_modelspace, 1);
}