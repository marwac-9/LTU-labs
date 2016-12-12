#version 330

//uniform mat4 V;
uniform vec2 screenSize;
uniform vec3 LightPosition_worldspace;
uniform vec3 CameraPos;

uniform sampler2D diffuseSampler;
uniform sampler2D positionSampler;
uniform sampler2D normalsSampler;
uniform sampler2D diffIntAmbIntShinSpecIntSampler;

uniform float lightRadius;
uniform float lightPower;
uniform vec3 lightColor;

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
	vec4 MatPropertiesDiffAmbShinSpec = texture(diffIntAmbIntShinSpecIntSampler, TexCoord);

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

	//float visibility = ShadowContribution(ShadowCoord.xy, ShadowCoord.z);
	//float depth = texture( shadowMapSampler, ShadowCoord.xy).r;
	//float bias = 0.0005f;

	//float visibility = 1.0;

	//if ( depth  <  ShadowCoord.z - bias){
	//	visibility = 0.5f;
	//}
	float radius = lightRadius - 0.5;
	//Linear Attenuation, based on distance.
	//Distance is divided by the max radius of the light which must be <= scale of the light mesh
	//float attenuation = max((1.0f - distance / (lightRadius - 0.5f)), 0.0);
	//vec3 att = LightDirection_worldSpace / (lightRadius - 0.5);
	//float attenuation = max(0.0, (1.0 - dot(att, att)) / (distance*distance*distance));
	//float attenuation = max((1.0 - distance / radius) / (((distance*distance*distance*lightPower) / (radius*radius/10.0))), 0.0);
	float attenuation = max((1.0 - distance / radius) / (distance*distance), 0.0);
	//float attenuation = max(1.0 / (distance*distance), 0.0);
	//float attenuation = pow(max(1.0 - (distance * distance) / (radius * radius), 0.0), 2.0);
	//float attenuation = 1 / ((((distance / (1 - ((distance / lightRadius)*(distance / lightRadius)))) / lightRadius) + 1)*(((distance / (1 - ((distance / lightRadius)*(distance / lightRadius)))) / lightRadius) + 1));
	//float attenuation = max(0.0, (1.0 - dot(att, att)) / (distance*distance*distance));
	//float attenuation = max(1.0 - ((distance*distance) / ((lightRadius - 0.5)*(lightRadius - 0.5))), 0.0);
	//float attenuation = smoothstep(radius, 0, distance);
	//float compression = 3.0;
	//float attenuation = pow(smoothstep(radius, 0, distance), compression);
	//float attenuation = (1.0 / (0.3 + 0.007 * distance + 0.00008 * distance * distance));

	//values here are material properties of an object
	float Ambient = MatPropertiesDiffAmbShinSpec.x;
	float Diffuse = MatPropertiesDiffAmbShinSpec.y * cosTheta;
	float Specular = MatPropertiesDiffAmbShinSpec.z * pow(cosAlpha, MatPropertiesDiffAmbShinSpec.w);

	//color with point light only
	//light power is the diffuse intensity of an light
	color = vec4(MaterialDiffuseColor * lightColor * lightPower * (Ambient + Diffuse + Specular) * attenuation, 1.0);
	float brightness = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));
	if (brightness > 1.0)
		brightColor = color;
	else
		brightColor = vec4(0.0, 0.0, 0.0, 1.0);
	//color = 
	// Ambient : simulates indirect lighting
	//MaterialAmbientColor * MaterialDiffuseIntensityValue +
	// Diffuse : "color" of the object
	//MaterialDiffuseColor * lightColor * lightPower * cosTheta * attenuation * MaterialDiffuseIntensityValue +
	// Specular : reflective highlight, like a mirror
	//MaterialSpecularColor * lightColor * lightPower * pow(cosAlpha, shininess) * attenuation;
}