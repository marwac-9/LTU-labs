#version 420 core

in vec2 UV;
in vec3 Position_worldspace;
in vec3 Normal_worldspace;
in vec3 EyeDirection_worldspace;
in vec3 LightDirection_worldspace;

layout(location = 0) out vec4 color;
layout(location = 1) out vec4 brightColor;

layout(binding = 0) uniform sampler2D diffuseSampler;

uniform vec4 MaterialColorShininess;
uniform float specular;
uniform float diffuse;
uniform float ambient;

uniform float lightPower;
uniform vec3 lightColor;

void main(){

	// Material properties
	vec3 MaterialDiffuseColor = pow(texture2D(diffuseSampler, UV).rgb + MaterialColorShininess.rgb, vec3(2.2));

	vec3 normal = normalize( Normal_worldspace );

	// Direction of the light (from the fragment to the light)
	vec3 lightDirection = normalize(LightDirection_worldspace);

	float diffuseFactor = clamp(dot(normal, lightDirection), 0, 1);

	// Eye vector (towards the camera)
	vec3 vertexToCamera = normalize(EyeDirection_worldspace);
	// Direction in which the triangle reflects the light
	vec3 reflectedLightDir = reflect(-lightDirection, normal);

	float specularFactor = clamp(dot(vertexToCamera, reflectedLightDir), 0, 1);

	float Metallic = 1.0; //metallic
	float Diffuse = diffuse * diffuseFactor; //diffuse intensity
	float Specular = specular * pow(specularFactor, MaterialColorShininess.w); //specular intensity, specular shininess
	vec3 SpecularColor = mix(lightColor, MaterialDiffuseColor, Metallic); //roughness parameter and reflection map will help with black metallic objects 

	color = vec4(lightColor * lightPower * (MaterialDiffuseColor * (ambient + Diffuse) + SpecularColor * Specular), 0.1f);
	float brightness = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));
	if (brightness > 1.0)
		brightColor = color;
	else
		brightColor = vec4(0.0, 0.0, 0.0, 1.0);
}
