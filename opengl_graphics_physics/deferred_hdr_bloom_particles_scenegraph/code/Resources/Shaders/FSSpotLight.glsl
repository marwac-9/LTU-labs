#version 330

//uniform mat4 V;
uniform vec2 screenSize;
uniform vec3 LightPosition_worldspace;
uniform vec3 CameraPos;

uniform sampler2D diffuseSampler;
uniform sampler2D positionSampler;
uniform sampler2D normalsSampler;
uniform sampler2D metDiffIntShinSpecIntSampler;

uniform float lightRadius;
uniform float lightPower;
uniform vec3 lightColor;
uniform float ambient;

struct Attenuation
{
	float constant;
	float linear;
	float exponential;
};

uniform vec3 SpotLightDirection_worldspace;
uniform Attenuation spotAttenuation;
uniform float SpotLightInnerCutOff;
uniform float SpotLightOuterCutOff;

// Ouput data
layout(location = 0) out vec4 color;
layout(location = 1) out vec4 brightColor;

void main()
{
	vec2 TexCoord = gl_FragCoord.xy / screenSize;
	vec3 WorldPos = texture(positionSampler, TexCoord).xyz;
	vec3 MaterialDiffuseColor = texture(diffuseSampler, TexCoord).xyz;
	vec3 Normal_worldSpace = texture(normalsSampler, TexCoord).xyz;
	// Material properties
	vec4 MatPropertiesMetDiffShinSpec = texture(metDiffIntShinSpecIntSampler, TexCoord);

	// Vector that goes from the vertex to the camera, in world space.
	vec3 EyeDirection_worldSpace = CameraPos - WorldPos;

	// Vector that goes from the vertex to the light, in world space. M is ommited because it's identity.
	vec3 LightDirection_worldSpace = LightPosition_worldspace - WorldPos;

	// Distance to the light
	float distance = length(LightDirection_worldSpace);

	//vec3 Normal_cameraspace = (V * vec4(Normal_worldSpace,0)).xyz;
	// Normal of the computed fragment, in camera space
	vec3 n = normalize(Normal_worldSpace);
	// Direction of the light (from the fragment to the light)
	vec3 l = normalize(LightDirection_worldSpace);
	// Cosine of the angle between the normal and the light direction, 
	// clamped above 0
	//  - light is at the vertical of the triangle -> 1
	//  - light is perpendicular to the triangle -> 0
	//  - light is behind the triangle -> 0
	float cosTheta = max(dot(n, l), 0.0);

	// Eye vector (towards the camera)
	vec3 E = normalize(EyeDirection_worldSpace);
	// Direction in which the triangle reflects the light
	vec3 R = reflect(-l, n);
	// Cosine of the angle between the Eye vector and the Reflect vector,
	// clamped to 0
	//  - Looking into the reflection -> 1
	//  - Looking elsewhere -> < 1
	float cosAlpha = max(dot(E, R), 0.0);

	float spotTetha = clamp(dot(l, SpotLightDirection_worldspace), 0.0, 1.0);
	float epsilon = (SpotLightInnerCutOff - SpotLightOuterCutOff);
	float intensity = clamp((spotTetha - SpotLightOuterCutOff) / epsilon, 0.0, 1.0);

	float radius = lightRadius - 0.5;

	float attenuation = 1.0 / (spotAttenuation.constant + spotAttenuation.linear * distance + spotAttenuation.exponential * distance * distance);
	//attenuation = max((1.0 - distance / radius) * attenuation, 0.0); //soften the hard edge at the end of the spot light radius

	float Metallic = MatPropertiesMetDiffShinSpec.x;
	float Diffuse = MatPropertiesMetDiffShinSpec.y * cosTheta;
	float Specular = MatPropertiesMetDiffShinSpec.z * pow(cosAlpha, MatPropertiesMetDiffShinSpec.w);
	vec3 SpecularColor = mix(vec3(1.0), MaterialDiffuseColor, Metallic);

	color = vec4((lightColor * lightPower * (MaterialDiffuseColor * (ambient + Diffuse) + SpecularColor * Specular) * attenuation) * intensity, 1.0);
	float brightness = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));
	if (brightness > 1.0)
		brightColor = color;
	else
		brightColor = vec4(0.0, 0.0, 0.0, 1.0);
}