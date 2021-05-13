#version 420 core

in vec2 UV;
in vec4 particlecolor;

// Ouput data
layout(location = 0) out vec4 color;
layout(location = 1) out vec4 brightColor;

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

layout(binding = 0) uniform sampler2D myTextureSampler;
layout(binding = 1) uniform sampler2D depthTextureSampler;
uniform float softScale;
uniform float contrastPower;

void main(){

	vec2 TexCoord = gl_FragCoord.xy / screenSize;

	float sceneDepth = texture(depthTextureSampler, TexCoord).r;
    sceneDepth = 2.0 * sceneDepth - 1.0;
    sceneDepth = 2.0 * near * far / (far + near - sceneDepth * (far - near));

	float particleDepth = gl_FragCoord.z;
	particleDepth = 2.0 * particleDepth - 1.0;
    particleDepth = 2.0 * near * far / (far + near - particleDepth * (far - near));

	if(particleDepth > sceneDepth)
		discard;

	float fade = (sceneDepth-particleDepth)*softScale;
	bool IsAboveHalf = fade > 0.5;	
    float ToRaise = clamp(2*(IsAboveHalf ? 1-fade : fade), 0.0, 1.0);
    fade = 0.5*pow(ToRaise, contrastPower); 
    fade = IsAboveHalf ? 1-fade : fade;

	color = texture( myTextureSampler, UV ) * particlecolor * vec4(1.0, 1.0, 1.0, fade);
	float brightness = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));
	if (brightness > 1.0)
		brightColor = color;
	else
		brightColor = vec4(0.0, 0.0, 0.0, 1.0);
}