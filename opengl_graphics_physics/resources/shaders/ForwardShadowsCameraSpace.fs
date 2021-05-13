#version 420 core

// Interpolated values from the vertex shaders
in vec2 UV;
in vec3 Position_worldspace;
in vec3 Normal_cameraspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace;
in vec4 ShadowCoord;

// Ouput data
layout(location = 0) out vec3 color;

// Values that stay constant for the whole mesh.
layout(binding = 0) uniform sampler2D myTextureSampler;
layout(binding = 10) uniform sampler2D shadowMapSampler;
uniform mat4 MV;
uniform vec3 LightPosition_worldspace;
uniform vec4 MaterialColorShininess;
uniform float specular;
uniform float diffuse;
uniform float ambient;

float linstep(float low, float high, float v){
    return clamp((v-low)/(high-low), 0.0, 1.0);
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

float ShadowContribution(vec2 LightTexCoord, float DistanceToLight)  
{  
	// Read the moments from the variance shadow map.  
	vec2 moments = texture2D(shadowMapSampler, LightTexCoord ).rg;
	// Compute the Chebyshev upper bound.  
	return ChebyshevUpperBound(moments, DistanceToLight);  
}  

void main(){

	// Light emission properties
	// You probably want to put them as uniforms
	vec3 LightColor = vec3(1,1,1);
	float LightPower = 1.f;
	
	// Material properties
	vec3 MaterialDiffuseColor = texture2D( myTextureSampler, UV ).rgb + MaterialColorShininess.rgb;

	// Distance to the light
	float distance = length( LightPosition_worldspace - Position_worldspace );

	// Normal of the computed fragment, in camera space
	vec3 n = normalize( Normal_cameraspace );
	// Direction of the light (from the fragment to the light)
	vec3 l = normalize( LightDirection_cameraspace );
	// Cosine of the angle between the normal and the light direction, 
	// clamped above 0
	//  - light is at the vertical of the triangle -> 1
	//  - light is perpendicular to the triangle -> 0
	//  - light is behind the triangle -> 0
	float cosTheta = clamp( dot( n,l ), 0,1 );
	
	// Eye vector (towards the camera)
	vec3 E = normalize(EyeDirection_cameraspace);
	// Direction in which the triangle reflects the light
	vec3 R = reflect(-l,n);
	// Cosine of the angle between the Eye vector and the Reflect vector,
	// clamped to 0
	//  - Looking into the reflection -> 1
	//  - Looking elsewhere -> < 1
	float cosAlpha = clamp( dot( E,R ), 0,1 );
	
	float visibility = ShadowContribution(ShadowCoord.xy,ShadowCoord.z);
	//float depth = texture( shadowMapSampler, ShadowCoord.xy).r;
	//float bias = 0.0005f;

	//float visibility = 1.0;

	//if ( depth  <  ShadowCoord.z - bias){
	//	visibility = 0.5f;
	//}
	float Ambient = ambient;
	float Diffuse = diffuse * cosTheta;
	float SpecularColor = specular * pow(cosAlpha, MaterialColorShininess.w);

	//1 directional
	float totalLight = (Ambient + Diffuse + SpecularColor);

	color = MaterialDiffuseColor * LightColor * LightPower * totalLight;

}