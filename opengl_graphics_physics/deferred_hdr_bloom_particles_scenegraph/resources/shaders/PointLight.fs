#version 420

layout(binding = 0) uniform sampler2D positionSampler;
layout(binding = 1) uniform sampler2D diffuseSampler;
layout(binding = 2) uniform sampler2D normalsSampler;
layout(binding = 3) uniform sampler2D metDiffIntShinSpecIntSampler;

layout(std140, binding = 1) uniform LBVars
{
	vec3 lightInvDir;
	float shadowTransitionSize;
	float outerCutOff;
	float innerCutOff;
	float lightRadius;
	float lightPower;
	vec3 lightColor;
	float ambient;
	mat4 depthBiasMVP;
	mat4 MVP;
	vec3 lightPosition;
	float constant;
	float linear;
	float exponential;
};

layout(std140, binding = 2) uniform CBVars
{
	vec2 screenSize;
	float near;
	float far;
	vec3 cameraPos;
};

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
	vec3 EyeDirection_worldSpace = cameraPos - WorldPos;

	// Vector that goes from the vertex to the light, in world space. M is ommited because it's identity.
	vec3 LightDirection_worldSpace = lightPosition - WorldPos;

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

	float radius = lightRadius * 0.97;
	//float attenuation = 1.0/(correctedDistance); //1.0 / (constant + linear * correctedDistance + exponential * correctedDistance * correctedDistance);
	float attenuation = 1.0 / (constant + linear * distance + exponential * distance * distance);
	attenuation = max((1.0 - distance / radius) * attenuation, 0.0);

	//attenuation research
	//Linear Attenuation, based on distance.
	//Distance is divided by the max radius of the light which must be <= scale of the light mesh
	//float attenuation = max((1.0f - distance / (lightRadius - 0.5f)), 0.0);
	//vec3 att = LightDirection_worldSpace / (lightRadius - 0.5);
	//float attenuation = max(0.0, (1.0 - dot(att, att)) / (distance*distance*distance));
	//float attenuation = max((1.0 - distance / radius) / (((distance*distance*distance*lightPower) / (radius*radius/10.0))), 0.0);
	///float attenuation = max((1.0 - distance / radius) / (distance*distance), 0.0);
	//float attenuation = max(1.0 / (distance*distance), 0.0);
	//float attenuation = pow(max(1.0 - (distance * distance) / (radius * radius), 0.0), 2.0);
	//float attenuation = 1 / ((((distance / (1 - ((distance / lightRadius)*(distance / lightRadius)))) / lightRadius) + 1)*(((distance / (1 - ((distance / lightRadius)*(distance / lightRadius)))) / lightRadius) + 1));
	//float attenuation = max(0.0, (1.0 - dot(att, att)) / (distance*distance*distance));
	//float attenuation = max(1.0 - ((distance*distance) / ((lightRadius - 0.5)*(lightRadius - 0.5))), 0.0);
	//float attenuation = smoothstep(radius, 0, distance);
	//float compression = 3.0;
	//float attenuation = pow(smoothstep(radius, 0, distance), compression);
	//float attenuation = (1.0 / (0.3 + 0.007 * distance + 0.00008 * distance * distance));


	float Metallic = MatPropertiesMetDiffShinSpec.x;
	float Diffuse = MatPropertiesMetDiffShinSpec.y * cosTheta;
	float Specular = MatPropertiesMetDiffShinSpec.z * pow(cosAlpha, MatPropertiesMetDiffShinSpec.w);
	vec3 SpecularColor = mix(vec3(1.0), MaterialDiffuseColor, Metallic);

	color = vec4(lightColor * lightPower * (MaterialDiffuseColor * (ambient + Diffuse) + SpecularColor * Specular) * attenuation, 1.0);
	float brightness = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));
	if (brightness > 1.0)
		brightColor = color;
	else
		brightColor = vec4(0.0, 0.0, 0.0, 1.0);
}