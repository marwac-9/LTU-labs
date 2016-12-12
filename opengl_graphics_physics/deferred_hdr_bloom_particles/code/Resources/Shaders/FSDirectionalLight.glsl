#version 330

//in vec4 ShadowCoord;
//in vec2 UV;

//uniform mat4 V;
uniform vec2 screenSize;
uniform vec3 LightInvDirection_worldspace;
uniform vec3 CameraPos;

uniform sampler2D diffuseSampler;
uniform sampler2D positionSampler;
uniform sampler2D normalsSampler;
//uniform sampler2D shadowMapSampler;

uniform sampler2D diffIntAmbIntShinSpecIntSampler;

uniform float lightPower;
uniform vec3 lightColor;

// Ouput data
layout(location = 0) out vec4 color;
layout(location = 1) out vec4 brightColor;

float linstep(float low, float high, float v){
	return clamp((v - low) / (high - low), 0.0, 1.0);
}

float ChebyshevUpperBound(vec2 moments, float t)
{
	// One-tailed inequality valid if t > moments.x  
	float p = (t <= moments.x) ? 1 : 0;
	//float p = smoothstep(t-0.02, t, moments.x);

	// Compute variance.  
	float minVariance = 0.0000005;
	//float minVariance = -0.001;
	float variance = moments.y - (moments.x*moments.x);
	variance = max(variance, minVariance);

	// Compute probabilistic upper bound.  
	float d = t - moments.x;
	float p_max = variance / (variance + d*d);

	p_max = smoothstep(0.2, 1.0, p_max);
	//p_max = linstep(0.2, 1.0, p_max);

	return max(p, p_max);
	//return clamp(max(p, p_max), 0.0, 1.0);
}
/*
float ShadowContribution(vec2 LightTexCoord, float DistanceToLight)
{
	// Read the moments from the variance shadow map.  
	vec2 moments = texture2D(shadowMapSampler, LightTexCoord).rg;
	// Compute the Chebyshev upper bound.  
	return ChebyshevUpperBound(moments, DistanceToLight);
}
*/

void main()
{
	vec2 TexCoord = gl_FragCoord.xy / screenSize;
	vec3 WorldPos = texture(positionSampler, TexCoord).xyz;
	vec3 MaterialDiffuseColor = texture(diffuseSampler, TexCoord).xyz;
	vec3 Normal_worldSpace = texture(normalsSampler, TexCoord).xyz;
	// Material properties
	vec4 MatPropertiesDiffAmbShinSpec = texture(diffIntAmbIntShinSpecIntSampler, TexCoord);

	// Vector that goes from the vertex to the camera, in world space.
	vec3 EyeDirection_worldSpace = CameraPos - WorldPos;

	// Normal of the computed fragment, in camera space
	vec3 n = normalize(Normal_worldSpace);
	// Direction of the light (from the vertex to the light)
	vec3 l = normalize(LightInvDirection_worldspace);
	// Cosine of the angle between the normal and the light direction, 
	// clamped above 0
	//  - light is at the vertical of the triangle -> 1
	//  - light is perpendicular to the triangle -> 0
	//  - light is behind the triangle -> 0
	float cosTheta = max(dot(n, l), 0);

	// Eye vector (towards the camera)
	vec3 E = normalize(EyeDirection_worldSpace);
	// Direction in which the triangle reflects the light
	vec3 R = reflect(-l, n);
	// Cosine of the angle between the Eye vector and the Reflect vector,
	// clamped to 0
	//  - Looking into the reflection -> 1
	//  - Looking elsewhere -> < 1
	float cosAlpha = max(dot(E, R), 0);

	//float visibility = ShadowContribution(ShadowCoord.xy, ShadowCoord.z);
	//float depth = texture( shadowMapSampler, ShadowCoord.xy).r;
	//float bias = 0.0005f;

	//float visibility = 1.0;

	//if ( depth  <  ShadowCoord.z - bias){
	//	visibility = 0.5f;
	//}
	//vec3 Ambient = lightColor * lightPower * MaterialAmbientIntensity;
	//vec3 Diffuse = lightColor * lightPower * MaterialDiffuseIntensityValue * cosTheta;
	//vec3 SpecularColor = MaterialSpecularColor * lightColor * lightPower * pow(cosAlpha, shininess);
	//specular sent to shader should be only the specular strength but we can try to specify color as well to fake translucent paint effect
	//these are only light calculations
	//we add the texture color later

	float Ambient = MatPropertiesDiffAmbShinSpec.x;
	float Diffuse = MatPropertiesDiffAmbShinSpec.y * cosTheta;
	float SpecularColor = MatPropertiesDiffAmbShinSpec.z * pow(cosAlpha, MatPropertiesDiffAmbShinSpec.w);

	//color with directional only
	color = vec4(MaterialDiffuseColor * lightColor * lightPower * (Ambient + Diffuse + SpecularColor), 1.0);
	float brightness = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));
	if (brightness > 1.0)
		brightColor = color;
	else
		brightColor = vec4(0.0, 0.0, 0.0, 1.0);
		// Ambient : simulates indirect lighting
		//MaterialAmbientColor * MaterialDiffuseIntensityValue +
		// Diffuse : "color" of the object
		//MaterialDiffuseColor * lightColor * lightPower * cosTheta * MaterialDiffuseIntensityValue +
		// Specular : reflective highlight, like a mirror
		//MaterialSpecularColor * lightColor * lightPower * pow(cosAlpha, shininess);
}