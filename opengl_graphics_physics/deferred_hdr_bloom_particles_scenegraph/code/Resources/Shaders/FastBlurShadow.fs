#version 330 core

in vec2 blurTextureCoords[11];

layout(location = 0) out vec2 color;

uniform sampler2D blurMapSampler;

void main(){

	color = vec2(0.0);
	
	color += texture(blurMapSampler, blurTextureCoords[0]).rg * 0.0093;
	color += texture(blurMapSampler, blurTextureCoords[1]).rg * 0.028002;
	color += texture(blurMapSampler, blurTextureCoords[2]).rg * 0.065984;
	color += texture(blurMapSampler, blurTextureCoords[3]).rg * 0.121703;
	color += texture(blurMapSampler, blurTextureCoords[4]).rg * 0.175713;
	color += texture(blurMapSampler, blurTextureCoords[5]).rg * 0.198596;
	color += texture(blurMapSampler, blurTextureCoords[6]).rg * 0.175713;
	color += texture(blurMapSampler, blurTextureCoords[7]).rg * 0.121703;
	color += texture(blurMapSampler, blurTextureCoords[8]).rg * 0.065984;
	color += texture(blurMapSampler, blurTextureCoords[9]).rg * 0.028002;
	color += texture(blurMapSampler, blurTextureCoords[10]).rg * 0.0093;
}