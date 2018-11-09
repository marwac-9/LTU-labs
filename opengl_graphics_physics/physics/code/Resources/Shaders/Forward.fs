#version 330 core

// Interpolated values from the vertex shaders
in vec2 UV;
in vec3 Position_worldspace;
in vec3 Normal_worldspace;
in vec3 EyeDirection_worldspace;
in vec3 PointLightDirectionFromVertex_worldspace;
in vec3 SpotLightDirectionFromVertex_worldspace;
// Ouput data
out vec3 color;

// Values that stay constant for the whole mesh.
uniform sampler2D myTextureSampler;
uniform vec4 MaterialProperties;
uniform vec3 MaterialColor;
uniform vec3 LightInvDirection_worldspace;
uniform float PointLightRadius;
uniform vec3 SpotLightDirection_worldspace;
uniform float SpotLightCutOff;
uniform float SpotLightOuterCutOff;
uniform float SpotLightRadius;

const float Ambient = 0.25; //is light property

struct Attenuation
{
	float constant;
	float linear;
	float exponential;
};

uniform Attenuation pointAttenuation;
uniform Attenuation spotAttenuation;

void main(){

	// Light emission properties
	// You probably want to put them as uniforms
	vec3 DirectionalLightColor = vec3(1,1,1);
	float DirectionalLightPower = 1.0;

	vec3 PointLightColor = vec3(1, 0, 0);
	float PointLightPower = 1.0;

	vec3 SpotLightColor = vec3(0, 0, 1);
	float SpotLightPower = 100.0;

	// Material properties
	vec3 MaterialDiffuseColor = texture2D(myTextureSampler, UV).rgb + MaterialColor;
	// Distance to the light
	float distance = length(PointLightDirectionFromVertex_worldspace);
	float distances = length(SpotLightDirectionFromVertex_worldspace);

	// Normal of the computed fragment
	vec3 n = normalize(Normal_worldspace);
	// Eye vector (towards the camera)
	vec3 E = normalize(EyeDirection_worldspace);

	// Direction of the light (from the fragment to the light)
	vec3 ld = normalize(LightInvDirection_worldspace);
	vec3 lp = normalize(PointLightDirectionFromVertex_worldspace);
	vec3 ls = normalize(SpotLightDirectionFromVertex_worldspace);

	float spotTetha = clamp(dot(ls,SpotLightDirection_worldspace), 0.0, 1.0);
	float epsilon = (SpotLightCutOff - SpotLightOuterCutOff);
	float intensity = clamp((spotTetha - SpotLightOuterCutOff) / epsilon, 0.0, 1.0);

	// Cosine of the angle between the normal and the light direction, 
	// clamped above 0
	//  - light is at the vertical of the triangle -> 1
	//  - light is perpendicular to the triangle -> 0
	//  - light is behind the triangle -> 0
	float cosThetad = clamp(dot(n,ld), 0,1);
	float cosThetap = clamp(dot(n,lp), 0,1);
	float cosThetas = clamp(dot(n,ls), 0,1);
	// Direction in which the triangle reflects the light
	vec3 Rd = reflect(-ld,n);
	vec3 Rp = reflect(-lp,n);
	vec3 Rs = reflect(-ls,n);
	// Cosine of the angle between the Eye vector and the Reflect vector,
	// clamped to 0
	//  - Looking into the reflection -> 1
	//  - Looking elsewhere -> < 1
	float cosAlphad = clamp(dot(E,Rd), 0,1);
	float cosAlphap = clamp(dot(E,Rp), 0,1);
	float cosAlphas = clamp(dot(E,Rs), 0,1);

	//float attenuationp = max((1.0 - distance / PointLightRadius) / (distance*distance), 0.0);
	//float attenuations = max((1.0 - distances / SpotLightRadius) / (distances*distances), 0.0);

	float attenuationp = pointAttenuation.constant + pointAttenuation.linear * distance + pointAttenuation.exponential * distance * distance;
	float attenuations = spotAttenuation.constant + spotAttenuation.linear * distance + spotAttenuation.exponential * distance * distance;

	float Metallic = MaterialProperties.x;
	float Diffused = MaterialProperties.y * cosThetad;
	float Diffusep = MaterialProperties.y * cosThetap;
	float Diffuses = MaterialProperties.y * cosThetas;// *intensity;
	float Speculard = MaterialProperties.z * pow(cosAlphad, MaterialProperties.w);
	float Specularp = MaterialProperties.z * pow(cosAlphap, MaterialProperties.w);
	float Speculars = MaterialProperties.z * pow(cosAlphas, MaterialProperties.w);// *intensity;
	vec3 SpecularColor = mix(vec3(1.0), MaterialDiffuseColor, Metallic); //roughness parameter and reflection map will help with black metallic objects 

	color = DirectionalLightColor * DirectionalLightPower * (MaterialDiffuseColor * (Ambient + Diffused) + SpecularColor * Speculard)
		+ PointLightColor * PointLightPower * (MaterialDiffuseColor * (Ambient + Diffusep) + SpecularColor * Specularp) / attenuationp
		+ (SpotLightColor * SpotLightPower * (MaterialDiffuseColor * (Ambient + Diffuses) + SpecularColor * Speculars) / attenuations) * intensity; //  * (1.0 - (1.0 - spotTetha) / (1.0 - SpotLightCutOff));

}