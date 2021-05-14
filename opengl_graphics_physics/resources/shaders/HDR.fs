#version 420 core

in vec2 UV;

layout(binding = 0) uniform sampler2D hdrBuffer;

layout(std140, binding = 3) uniform PBVars
{
	float gamma;
	float exposure;
	float brightness;
	float contrast;
	float bloomIntensity;
	bool hdrEnabled;
	bool bloomEnabled;
};

out vec4 color;

void main()
{
	vec3 hdrColor = texture(hdrBuffer, UV).rgb;
	if (hdrEnabled)
	{
		// reinhard
		// vec3 result = hdrColor / (hdrColor + vec3(1.0));
		// exposure
		vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
		// also gamma correct while we're at it       
		result = pow(result, vec3(1.0 / gamma));
		color = vec4(result, 1.0f);
	}
	else
	{
		vec3 result = pow(hdrColor, vec3(1.0 / gamma));
		color = vec4(result, 1.0);
	}
}