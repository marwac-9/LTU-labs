#version 420 core

in vec2 UV;

// Ouput data
layout(location = 0) out vec3 color;

layout(binding = 8) uniform sampler2D blurMapSampler;

//Gaussian Blur screen size
uniform vec2 scaleUniform;

void main(){
	vec3 blur = vec3(0.0);
	
	blur += texture(blurMapSampler, UV + vec2(-5.0*scaleUniform.x, -3.0*scaleUniform.y)).rgb * 0.0093;
	blur += texture(blurMapSampler, UV + vec2(-4.0*scaleUniform.x, -3.0*scaleUniform.y)).rgb * 0.028002;
	blur += texture(blurMapSampler, UV + vec2(-3.0*scaleUniform.x, -3.0*scaleUniform.y)).rgb * 0.065984;
	blur += texture(blurMapSampler, UV + vec2(-2.0*scaleUniform.x, -2.0*scaleUniform.y)).rgb * 0.121703;
	blur += texture(blurMapSampler, UV + vec2(-1.0*scaleUniform.x, -1.0*scaleUniform.y)).rgb * 0.175713;
	blur += texture(blurMapSampler, UV + vec2(0.0, 0.0)).rgb * 0.198596;
	blur += texture(blurMapSampler, UV + vec2(1.0*scaleUniform.x, 1.0*scaleUniform.y)).rgb * 0.175713;
	blur += texture(blurMapSampler, UV + vec2(2.0*scaleUniform.x, 2.0*scaleUniform.y)).rgb * 0.121703;
	blur += texture(blurMapSampler, UV + vec2(3.0*scaleUniform.x, -3.0*scaleUniform.y)).rgb * 0.065984;
	blur += texture(blurMapSampler, UV + vec2(4.0*scaleUniform.x, -3.0*scaleUniform.y)).rgb * 0.028002;
	blur += texture(blurMapSampler, UV + vec2(5.0*scaleUniform.x, -3.0*scaleUniform.y)).rgb * 0.0093;

	color = blur;
}