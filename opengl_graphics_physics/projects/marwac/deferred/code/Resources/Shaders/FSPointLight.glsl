#version 330

//uniform mat4 V;
uniform vec2 screenSize;
uniform vec3 LightPosition_worldspace;
uniform vec3 CameraPos;

uniform sampler2D diffuseSampler;
uniform sampler2D positionSampler;
uniform sampler2D normalsSampler;

uniform sampler2D diffIntAmbIntShinSampler;
uniform sampler2D materialColorSampler;
uniform sampler2D specularColorSampler;

uniform float lightRadius;
uniform float lightPower;
uniform vec3 lightColor;

// Ouput data
layout(location = 0) out vec3 color;

void main()
{
	vec2 TexCoord = gl_FragCoord.xy / screenSize;
	vec3 WorldPos = texture(positionSampler, TexCoord).xyz;
	vec3 MaterialDiffuseColor = texture(diffuseSampler, TexCoord).xyz;
	vec3 Normal_worldSpace = texture(normalsSampler, TexCoord).xyz;

	// Material properties
	vec3 MaterialColor = texture(materialColorSampler, TexCoord).xyz;
	vec3 MaterialSpecularColor = texture(specularColorSampler, TexCoord).xyz;
	vec3 MatPropertiesDiffAmbShin = texture(diffIntAmbIntShinSampler, TexCoord).xyz;
	float MaterialDiffuseIntensityValue = MatPropertiesDiffAmbShin.x;
	float shininess = MatPropertiesDiffAmbShin.z;
	//update the diffuse color with material color
	MaterialDiffuseColor = MaterialDiffuseColor + MaterialColor;
	float MaterialAmbientIntensity = MatPropertiesDiffAmbShin.y;
	vec3 MaterialAmbientColor = MaterialAmbientIntensity * MaterialDiffuseColor;

	// Distance to the light
	float distance = length(LightPosition_worldspace - WorldPos);

	// Vector that goes from the vertex to the camera, in world space.
	vec3 EyeDirection_worldSpace = CameraPos - WorldPos;

	// Vector that goes from the vertex to the light, in world space. M is ommited because it's identity.
	vec3 LightDirection_worldSpace = LightPosition_worldspace - WorldPos;

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
	float cosTheta = clamp(dot(n, l), 0, 1);

	// Eye vector (towards the camera)
	vec3 E = normalize(EyeDirection_worldSpace);
	// Direction in which the triangle reflects the light
	vec3 R = reflect(-l, n);
	// Cosine of the angle between the Eye vector and the Reflect vector,
	// clamped to 0
	//  - Looking into the reflection -> 1
	//  - Looking elsewhere -> < 1
	float cosAlpha = clamp(dot(E, R), 0, 1);

	//float visibility = ShadowContribution(ShadowCoord.xy, ShadowCoord.z);
	//float depth = texture( shadowMapSampler, ShadowCoord.xy).r;
	//float bias = 0.0005f;

	//float visibility = 1.0;

	//if ( depth  <  ShadowCoord.z - bias){
	//	visibility = 0.5f;
	//}

	//Linear Attenuation, based on distance.
	//Distance is divided by the max radius of the light which must be <= scale of the light mesh
	float attenuation = clamp((1.0f - distance / (lightRadius-0.5f)), 0.0, 1.0);
	//color with pointlight
	color = 
	// Ambient : simulates indirect lighting
	//MaterialAmbientColor * MaterialDiffuseIntensityValue +
	// Diffuse : "color" of the object
	MaterialDiffuseColor * lightColor * lightPower * cosTheta * attenuation * MaterialDiffuseIntensityValue +
	// Specular : reflective highlight, like a mirror
	MaterialSpecularColor * lightColor * lightPower * pow(cosAlpha, shininess) * attenuation;
}