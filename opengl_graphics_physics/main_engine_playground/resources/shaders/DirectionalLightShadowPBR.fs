#version 420

layout(binding = 0) uniform sampler2D positionSampler;
layout(binding = 1) uniform sampler2D albedoSampler;
layout(binding = 2) uniform sampler2D normalsSampler;
layout(binding = 3) uniform sampler2D aoRoughnessMetallic;
layout(binding = 4) uniform sampler2D shadowMapSampler;

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

// Ouput data
layout(location = 0) out vec4 color;
layout(location = 1) out vec4 brightColor;

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

float linstep(float low, float high, float v) {
	return clamp((v - low) / (high - low), 0.0, 1.0);
}

float ShadowContribution(vec2 LightTexCoord, float DistanceToLight)
{
	// Read the moments from the variance shadow map.  
	vec2 moments = texture2D(shadowMapSampler, LightTexCoord).rg;
	// Compute the Chebyshev upper bound.  

	// One-tailed inequality valid if DistanceToLight > moments.x  
	//float p = (DistanceToLight <= moments.x) ? 1 : 0;
	float p = step(DistanceToLight, moments.x);
	//float p = smoothstep(t-0.02, DistanceToLight, moments.x);

	// Compute variance.  
	float minVariance = 0.0000005;
	//float minVariance = -0.001;
	float variance = moments.y - (moments.x*moments.x);
	variance = max(variance, minVariance);

	// Compute probabilistic upper bound.  
	float d = DistanceToLight - moments.x;
	float p_max = variance / (variance + d * d);

	//p_max = smoothstep(0.1, 1.0, p_max);
	p_max = linstep(0.2, 1.0, p_max);

	return min(max(p, p_max), 1.0);
	//return clamp(max(p, p_max), 0.0, 1.0);
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
	
	// Direction of the light (from the vertex to the light)
	vec3 LightInvDir = normalize(lightInvDir);
	
	vec3 H = normalize(EyeDirection + LightInvDir);
	
	vec3 radiance = lightColor * lightPower;
	
	// calculate per-light radiance
	float NDF = DistributionGGX(Normal, H, roughness);
	float G   = GeometrySmith(Normal, EyeDirection, LightInvDir, roughness);
	vec3 F   = fresnelSchlick(max(dot(H, EyeDirection), 0.0), F0);
	
	// Cook-Torrance BRDF
	vec3 nominator    = NDF * G * F;
	float denominator = 4.0 * max(dot(Normal, EyeDirection), 0.0) * max(dot(Normal, LightInvDir), 0.0);
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
	float NdotL = max(dot(Normal, LightInvDir), 0.0);
	// add to outgoing radiance Lo
	Lo += (kD * Albedo / PI + specularV) * radiance * NdotL;
	
	
	//--------------- shadows --------------------//
	float distance = length(EyeDirectionWS);
	
	vec4 ShadowCoord = depthBiasMVP * vec4(WorldPos, 1);
	
	float visibility = 1.0;
	float shadowMapColor = 0.0;
	float shadowFadeFactor = 0.0;
	if (ShadowCoord.z <= 1.0 && ShadowCoord.y <= 1.0 && ShadowCoord.x <= 1.0 && ShadowCoord.z >= 0.0 && ShadowCoord.y >= 0.0 && ShadowCoord.x >= 0.0)
	{
		visibility = ShadowContribution(ShadowCoord.xy, ShadowCoord.z); //calculate visibility
		shadowFadeFactor = linstep(lightRadius - shadowTransitionSize, lightRadius, distance);
		visibility = mix(visibility, 1.0, shadowFadeFactor); //fade shadows out around us
		shadowMapColor = 1.0;
		//visibility = 1.0-visibility;
	}
	
	color = vec4(Lo * visibility, 1.0);
	
	float brightness = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));
	if (brightness > 1.0)
		brightColor = color;
	else
	brightColor = vec4(0.0, 0.0, 0.0, 1.0);
}