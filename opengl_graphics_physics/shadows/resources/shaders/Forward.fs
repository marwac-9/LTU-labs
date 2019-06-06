#version 420 core

// Interpolated values from the vertex shaders
in vec2 UV;
in vec3 Position_worldspace;
in vec3 Normal_worldspace;
in vec4 ShadowCoord;

// Ouput data
layout(location = 0) out vec3 color;

// Values that stay constant for the whole mesh.
layout(binding = 0) uniform sampler2D myTextureSampler;
layout(binding = 1) uniform sampler2D shadowMapSampler;
uniform vec4 MaterialColorShininess;
uniform vec3 LightInvDirection_worldspace;
uniform vec3 CameraPos;
uniform float shadowDistance;

uniform float transitionDistance;

float linstep(float low, float high, float v){
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

void main(){

	// Light emission properties
	// You probably want to put them as uniforms
	vec3 LightColor = vec3(1,1,1);
	float LightPower = 1.0f;
	float specular = 0.5;
	float diffuse = 1.0;
	float ambient = 0.25;
	// Material properties
	vec3 MaterialDiffuseColor = texture2D(myTextureSampler, UV).rgb + MaterialColorShininess.rgb;

	// Vector that goes from the vertex to the camera, in world space.
	vec3 EyeDirection_worldSpace = CameraPos - Position_worldspace;

	// Normal of the computed fragment, in camera space
	vec3 n = normalize( Normal_worldspace );
	// Direction of the light (from the fragment to the light)
	vec3 l = normalize( LightInvDirection_worldspace );
	// Cosine of the angle between the normal and the light direction, 
	// clamped above 0
	//  - light is at the vertical of the triangle -> 1
	//  - light is perpendicular to the triangle -> 0
	//  - light is behind the triangle -> 0
	float cosTheta = clamp( dot( n,l ), 0,1 );
	
	// Eye vector (towards the camera)
	vec3 E = normalize( EyeDirection_worldSpace );
	// Direction in which the triangle reflects the light
	vec3 R = reflect(-l,n);
	// Cosine of the angle between the Eye vector and the Reflect vector,
	// clamped to 0
	//  - Looking into the reflection -> 1
	//  - Looking elsewhere -> < 1
	float cosAlpha = clamp( dot( E,R ), 0,1 );

	float distance = length(EyeDirection_worldSpace);

	float visibility = 1.0;
	float shadowMapColor = 0.0;
	float shadowFadeFactor = 0.0;
	if (ShadowCoord.z <= 1.0 && ShadowCoord.y <= 1.0 && ShadowCoord.x <= 1.0 && ShadowCoord.z >= 0.0 && ShadowCoord.y >= 0.0 && ShadowCoord.x >= 0.0) //instead of using a white border on texture we simply ignore testing if any component is outside of shadow map texture
	{
		visibility = ShadowContribution(ShadowCoord.xy, ShadowCoord.z); //calculate visibility
		shadowFadeFactor = linstep(shadowDistance - transitionDistance, shadowDistance, distance); //fade shadows out around us
		visibility = mix(visibility, 1.0, shadowFadeFactor); //fade shadows out around us, we also need to fade shadows only if we want shadowmap to move with us
		//if we won't move them then no need for fading and then we can see them from afar
		shadowMapColor = 1.0;
	}
	
	//float depth = texture( shadowMapSampler, ShadowCoord.xy).r;
	//float bias = 0.0005f;

	//float visibility = 1.0;

	//if ( depth  <  ShadowCoord.z - bias){
	//	visibility = 0.5f;
	//}
	
	//just directional light
	float Metallic = 1.0;
	float Diffuse = diffuse * cosTheta;
	float Specular = specular * pow(cosAlpha, MaterialColorShininess.w);
	//vec3 SpecularColor = mix(MaterialDiffuseColor, vec3(1.0), MaterialProperties.x);
	vec3 SpecularColor = mix(LightColor, MaterialDiffuseColor, Metallic); //roughness parameter and reflection map will help with black metallic objects 

	color = LightColor * LightPower * (vec3(0.0, 0.2, 0.2) * shadowMapColor + vec3(0.0, 0.2, 0.0) * shadowFadeFactor + MaterialDiffuseColor * ambient + (MaterialDiffuseColor * Diffuse + SpecularColor * Specular) * visibility);
}