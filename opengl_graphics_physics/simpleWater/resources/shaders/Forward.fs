#version 420 core

in vec2 UV;
in vec3 Position_worldspace;
in vec3 Normal_worldspace;
in vec3 EyeDirection_worldspace;
in vec3 LightDirection_worldspace;

layout(location = 0) out vec4 color;
layout(location = 1) out vec4 brightColor;

layout(binding = 0) uniform sampler2D myTextureSampler;

uniform vec4 MaterialProperties;
uniform vec3 MaterialColor;

uniform float lightPower;
uniform vec3 lightColor;

const float Ambient = 0.25; //is light property

void main(){

	// Material properties
	vec3 MaterialDiffuseColor = texture2D(myTextureSampler, UV).rgb + MaterialColor;

	vec3 normal = normalize( Normal_worldspace );

	// Direction of the light (from the fragment to the light)
	vec3 lightDirection = normalize(LightDirection_worldspace);

	float diffuseFactor = clamp(dot(normal, lightDirection), 0, 1);

	// Eye vector (towards the camera)
	vec3 vertexToCamera = normalize(EyeDirection_worldspace);
	// Direction in which the triangle reflects the light
	vec3 reflectedLightDir = reflect(-lightDirection, normal);

	float specularFactor = clamp(dot(vertexToCamera, reflectedLightDir), 0, 1);

	float Metallic = MaterialProperties.x; //metallic
	float Diffuse = MaterialProperties.y * diffuseFactor; //diffuse intensity
	float Specular = MaterialProperties.z * pow(specularFactor, MaterialProperties.w); //specular intensity, specular shininess
	vec3 SpecularColor = mix(vec3(1.0), MaterialDiffuseColor, Metallic); //roughness parameter and reflection map will help with black metallic objects 

	color = vec4(lightColor * lightPower * (MaterialDiffuseColor * (Ambient + Diffuse) + SpecularColor * Specular), 0.1f);
	float brightness = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));
	if (brightness > 1.0)
		brightColor = color;
	else
		brightColor = vec4(0.0, 0.0, 0.0, 1.0);
}
