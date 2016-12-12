#version 150

uniform sampler2D	font_map;
uniform float      smoothness;

const float kGamma = 2.2;

in vec2 vTexCoord0;
in vec4 vColor;

out vec4 oColor;

void main()
{
	// retrieve signed distance
	float sdf = texture( font_map, vTexCoord0.xy ).r;

	// perform adaptive anti-aliasing of the edges
	float w = clamp( smoothness * (abs(dFdx(vTexCoord0.x)) + abs(dFdy(vTexCoord0.y))), 0.0, 0.5);
	float a = smoothstep(0.5-w, 0.5+w, sdf);

	// gamma correction for linear attenuation
	a = pow(a, 1.0/kGamma);

	// final color
	oColor.rgb = vColor.rgb;
	oColor.a = vColor.a * a;
}