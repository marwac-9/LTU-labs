#version 420

layout(binding = 0) uniform sampler2D positionSampler;
layout(binding = 1) uniform sampler2D albedoSampler;
layout(binding = 2) uniform sampler2D normalsSampler;
layout(binding = 3) uniform sampler2D specularSampler;

layout(std140, binding = 1) uniform LBVars
{
	mat4 depthBiasMVP;
	vec3 lightInvDir;
	float shadowTransitionSize;
	float outerCutOff;
	float innerCutOff;
	float lightRadius;
	float lightPower;
	vec3 lightColor;
	float ambient;
	float diffuse;
	float specular;
	vec3 lightPosition;
	float constant;
	float linear;
	float exponential;
};

layout(std140, binding = 2) uniform CBVars
{
	mat4 VP;
	vec2 screenSize;
	float near;
	float far;
	vec3 cameraPos;
	vec3 cameraUp;
	vec3 cameraRight;
	vec3 cameraForward;
};

// Ouput data
layout(location = 0) out vec4 color;
layout(location = 1) out vec4 brightColor;

void main()
{
	vec2 TexCoord = gl_FragCoord.xy / screenSize;
	vec3 WorldPos = texture(positionSampler, TexCoord).xyz;
	vec3 MaterialDiffuseColor = pow(texture(albedoSampler, TexCoord).xyz, vec3(2.2));
	vec3 Normal_worldSpace = texture(normalsSampler, TexCoord).xyz;
	vec4 AoRoughnessMetallicShininess = texture(specularSampler, TexCoord);

	// Vector that goes from the vertex to the camera, in world space.
	vec3 EyeDirection_worldSpace = cameraPos - WorldPos;

	// Normal of the computed fragment, in camera space
	vec3 n = normalize(Normal_worldSpace);
	// Direction of the light (from the vertex to the light)
	vec3 l = normalize(lightInvDir);
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

	float Metallic = AoRoughnessMetallicShininess.z;
	float Diffuse = diffuse * cosTheta;
	float Specular = specular * pow(cosAlpha, AoRoughnessMetallicShininess.w);
	vec3 SpecularColor = mix(lightColor, MaterialDiffuseColor, Metallic);
	float smoothness = 1.0 - AoRoughnessMetallicShininess.y;

	color = vec4(lightColor * lightPower * (MaterialDiffuseColor * (ambient + Diffuse) + SpecularColor * Specular * smoothness), 1.0);
	float brightness = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));
	if (brightness > 1.0)
		brightColor = color;
	else
		brightColor = vec4(0.0, 0.0, 0.0, 1.0);
}