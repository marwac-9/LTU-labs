#version 330 core

// Interpolated values from the vertex shaders
in vec2 UV;
in vec3 Position_worldspace;
in vec3 Normal_cameraspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace;

// Ouput data
layout(location = 0) out vec3 color;

// Values that stay constant for the whole mesh.
uniform sampler2D myTextureSampler;
uniform vec3 LightPosition_worldspace;
uniform vec4 MaterialColorShininess;


void main(){

	// Light emission properties
	// You probably want to put them as uniforms
	vec3 LightColor = vec3(1,1,1);
	float LightPower = 1.f;
	float specular = 0.5;
	float diffuse = 1.0;
	float ambient = 0.25;

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
	
	float Ambient = ambient;
	float Diffuse = diffuse * cosTheta;
	float SpecularColor = specular * pow(cosAlpha, MaterialColorShininess.w);

	//1 directional, 1 point
	float totalLight = (Ambient + Diffuse + SpecularColor) + (Ambient + Diffuse + SpecularColor) / (distance*distance);

	color = MaterialDiffuseColor * LightColor * LightPower * totalLight;
}