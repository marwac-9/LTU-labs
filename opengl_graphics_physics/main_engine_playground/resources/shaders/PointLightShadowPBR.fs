#version 420

layout(binding = 0) uniform sampler2D positionSampler;
layout(binding = 1) uniform sampler2D albedoSampler;
layout(binding = 2) uniform sampler2D normalsSampler;
layout(binding = 3) uniform sampler2D aoRoughnessMetallic;
layout(binding = 4) uniform samplerCube shadowMapSampler;

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


const vec3 sampleOffsetDirections[20] = vec3[]
(
	vec3(1, 1, 1), vec3(1, -1, 1), vec3(-1, -1, 1), vec3(-1, 1, 1),
	vec3(1, 1, -1), vec3(1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
	vec3(1, 1, 0), vec3(1, -1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
	vec3(1, 0, 1), vec3(-1, 0, 1), vec3(1, 0, -1), vec3(-1, 0, -1),
	vec3(0, 1, 1), vec3(0, -1, 1), vec3(0, -1, -1), vec3(0, 1, -1)
);

// Ouput data
layout(location = 0) out vec4 color;
layout(location = 1) out vec4 brightColor;

float linstep(float low, float high, float v) {
	return clamp((v - low) / (high - low), 0.0, 1.0);
}

float ShadowContribution(vec3 LightToFragment, float viewDistance, vec3 fragPos)
{
	float currentDepth = length(LightToFragment);

	float shadow = 0.0;
	float bias = 0.15;
	int samples = 20;
	float diskRadius = 0.05;//(1.0 + (viewDistance / far_plane)) / 25.0; //this viewDistance mod brings back acne at distance

	for (int i = 0; i < samples; ++i)
	{
		float closestDepth = texture(shadowMapSampler, LightToFragment + sampleOffsetDirections[i] * diskRadius).r;
		closestDepth *= lightRadius;   // undo mapping [0;1]
		if (currentDepth - bias > closestDepth)
			shadow += 1.0;
	}

	shadow /= float(samples);
	
	return shadow;
	//float closestDepth = texture(shadowMapSampler, LightToFragment).r;
	
	//closestDepth *= far_plane;

	

	//float bias = 0.05;
	//float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

	//return shadow;	
}

float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 F0);

const float PI = 3.14159265359;

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}

void main()
{
	vec2 TexCoord = gl_FragCoord.xy / screenSize;
	vec3 WorldPos = texture(positionSampler, TexCoord).xyz;
	vec3 Albedo = pow(texture(albedoSampler, TexCoord).xyz, vec3(2.2));
	vec3 Normal_worldSpace = texture(normalsSampler, TexCoord).xyz;
	vec3 AoRoughnessMetallic = texture(aoRoughnessMetallic, TexCoord).xyz;

	float ao = AoRoughnessMetallic.x;
	float roughness = clamp(AoRoughnessMetallic.y, 0.1, 0.9);
	float metallic = clamp(AoRoughnessMetallic.z, 0.1, 0.9);

	// Vector that goes from the vertex to the camera
	vec3 EyeDirectionWS = (cameraPos - WorldPos);

	vec3 EyeDirection = normalize(EyeDirectionWS);
    
    vec3 Normal = normalize(Normal_worldSpace);

    vec3 R = reflect(-EyeDirection, Normal); 
    
	// calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)
	vec3 F0 = vec3(0.04); 
    F0 = mix(F0, Albedo, metallic);
    
    // reflectance equation
    vec3 Lo = vec3(0.0);

	// Vector that goes from the vertex to the light, in world space. M is ommited because it's identity.
	vec3 LightDirection_worldSpace = lightPosition - WorldPos;

	// Distance to the light
	float distance = length(LightDirection_worldSpace);
	
    vec3 LightDirection = normalize(LightDirection_worldSpace);
    
    vec3 H = normalize(EyeDirection + LightDirection);

	float radius = lightRadius * 0.97;
	float attenuation = 1.0 / (constant + linear * distance + exponential * distance * distance);
	attenuation = max((1.0 - distance / radius) * attenuation, 0.0);
    
    vec3 radiance = lightColor * lightPower;
    
    // calculate per-light radiance
    float NDF = DistributionGGX(Normal, H, roughness);
    float G   = GeometrySmith(Normal, EyeDirection, LightDirection, roughness);
    vec3  F   = fresnelSchlick(max(dot(H, EyeDirection), 0.0), F0);

	// Cook-Torrance BRDF
    vec3 nominator    = NDF * G * F;
    float denominator = 4.0 * max(dot(Normal, EyeDirection), 0.0) * max(dot(Normal, LightDirection), 0.0);
    vec3 specularV     = nominator / max(denominator, 0.001);  
	
	// kS is equal to Fresnel
	vec3 kS = F;
	// for energy conservation, the diffuse and specular light can't
    // be above 1.0 (unless the surface emits light); to preserve this
    // relationship the diffuse component (kD) should equal 1.0 - kS.
    vec3 kD = vec3(1.0) - kS;
	// multiply kD by the inverse metalness such that only non-metals 
    // have diffuse lighting, or a linear blend if partly metal (pure metals
	// have no diffuse light).
    kD     *= 1.0 - metallic;

	// scale light by NdotL
    float NdotL = max(dot(Normal, LightDirection), 0.0);
	// add to outgoing radiance Lo
    Lo += (kD * Albedo / PI + specularV) * radiance * NdotL;

	//--------------- shadows --------------------//
	float visibility = 1.0 - ShadowContribution(-LightDirection_worldSpace, length(EyeDirectionWS), WorldPos);

    color = vec4((Lo * visibility) * attenuation, 1.0);

	float brightness = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));
	if (brightness > 1.0)
		brightColor = color;
	else
		brightColor = vec4(0.0, 0.0, 0.0, 1.0);
}