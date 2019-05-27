#version 420 core

in vec3 UVdirection;

layout(location = 0) out vec4 DiffuseOut;

layout(binding = 0) uniform sampler2D equirectangularMap;

const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main(){
	vec2 uv = SampleSphericalMap(normalize(UVdirection)); // make sure to normalize direction
	DiffuseOut = vec4(texture(equirectangularMap, uv).rgb,1);
}