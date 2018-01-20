#version 330 core

in vec2 UV;

uniform sampler2D hdrBuffer;
uniform sampler2D bloomBuffer;
uniform bool hdr;
uniform float exposure;
uniform float gamma;
uniform float bloomIntensity;

out vec4 color;

void main()
{
	vec3 hdrColor = texture(hdrBuffer, UV).rgb;
	vec3 bloomColor = texture(bloomBuffer, UV).rgb;

	hdrColor += bloomColor * bloomIntensity; // additive blending
	
	if (hdr)
	{
		// tone mapping
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