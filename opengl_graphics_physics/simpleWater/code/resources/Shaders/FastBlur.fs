#version 330 core

in vec2 blurTextureCoords[11];

layout(location = 0) out vec3 color;

uniform sampler2D blurMapSampler;

void main(){

	color = vec3(0.0);
	
	color += texture(blurMapSampler, blurTextureCoords[0]).rgb * 0.0093;
	color += texture(blurMapSampler, blurTextureCoords[1]).rgb * 0.028002;
	color += texture(blurMapSampler, blurTextureCoords[2]).rgb * 0.065984;
	color += texture(blurMapSampler, blurTextureCoords[3]).rgb * 0.121703;
	color += texture(blurMapSampler, blurTextureCoords[4]).rgb * 0.175713;
	color += texture(blurMapSampler, blurTextureCoords[5]).rgb * 0.198596;
	color += texture(blurMapSampler, blurTextureCoords[6]).rgb * 0.175713;
	color += texture(blurMapSampler, blurTextureCoords[7]).rgb * 0.121703;
	color += texture(blurMapSampler, blurTextureCoords[8]).rgb * 0.065984;
	color += texture(blurMapSampler, blurTextureCoords[9]).rgb * 0.028002;
	color += texture(blurMapSampler, blurTextureCoords[10]).rgb * 0.0093;
}