#version 420 core

in vec2 UV;

uniform sampler2D hdrBuffer;
uniform sampler2D bloomBuffer;
//uniform vec3 vHSV;

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

vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main()
{
	vec3 hdrColor = texture(hdrBuffer, UV).rgb;
	vec3 bloomColor = texture(bloomBuffer, UV).rgb;

	if(bloomEnabled) 
	{
		hdrColor += bloomColor * bloomIntensity; // additive blending
	}
	
	if (hdrEnabled)
	{
		//vec3 fragRGB = hdrColor;
		//vec3 fragHSV = rgb2hsv(fragRGB).xyz;
		//fragHSV.x += vHSV.x / 360.0;
		//fragHSV.yz *= vHSV.yz;
		//fragHSV.xyz = mod(fragHSV.xyz, 1.0);
		//fragRGB = hsv2rgb(fragHSV);
		//hdrColor = fragRGB;

		// tone mapping
		// reinhard
		// vec3 result = hdrColor / (hdrColor + vec3(1.0));
		// exposure
		vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
		// also gamma correct while we're at it       
		result = pow(result, vec3(1.0 / gamma));
		result = (result - 0.5) * contrast + 0.5 + brightness;

		color = vec4(result, 1.0f);
	}
	else
	{
		vec3 result = pow(hdrColor, vec3(1.0 / gamma));

		color = vec4(result, 1.0);
	}
}