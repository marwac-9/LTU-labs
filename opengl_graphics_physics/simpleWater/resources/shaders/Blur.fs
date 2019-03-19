#version 420 core

in vec2 UV;

// Ouput data
layout(location = 0) out vec3 color;

layout(binding = 5) uniform sampler2D blurMapSampler;

//Gaussian Blur screen size
uniform vec2 scaleUniform;

void main(){
	vec3 blur = vec3(0.0);
		
	blur += texture(blurMapSampler, UV + vec2(-3.0*scaleUniform.x, -3.0*scaleUniform.y)).rgb * 0.015625;
	blur += texture(blurMapSampler, UV + vec2(-2.0*scaleUniform.x, -2.0*scaleUniform.y)).rgb * 0.09375;
	blur += texture(blurMapSampler, UV + vec2(-1.0*scaleUniform.x, -1.0*scaleUniform.y)).rgb * 0.234375;
	blur += texture(blurMapSampler, UV + vec2(0.0, 0.0)).rgb * 0.3125;
	blur += texture(blurMapSampler, UV + vec2(1.0*scaleUniform.x, 1.0*scaleUniform.y)).rgb * 0.234375;
	blur += texture(blurMapSampler, UV + vec2(2.0*scaleUniform.x, 2.0*scaleUniform.y)).rgb * 0.09375;
	blur += texture(blurMapSampler, UV + vec2(3.0*scaleUniform.x, -3.0*scaleUniform.y)).rgb * 0.015625;

	color = blur;
}