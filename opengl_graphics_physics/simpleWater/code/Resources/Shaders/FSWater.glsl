#version 330 core

in vec3 Position_worldspace;
in vec3 EyeDirection_worldspace;
in vec3 LightDirection_worldspace;
in vec3 clipUV;
in vec2 UV;
in float moveFactor;

layout(location = 0) out vec4 color;
layout(location = 1) out vec4 brightColor;

uniform sampler2D reflectionSampler;
uniform sampler2D refractionSampler;
uniform sampler2D normalMapSampler;
uniform sampler2D dudvMapSampler;
uniform sampler2D depthMapSampler;

uniform float shininess;
uniform float specularIntensity;
uniform vec2 screenSize;
uniform float near;
uniform float far;
uniform vec2 tiling;
uniform vec3 waterColor;
uniform float lightPower;
uniform vec3 lightColor;

uniform float waveStrength;
uniform float maxDepthTransparent;
uniform float waterRefractionDepth;
uniform float fresnelAdjustment;
uniform float softenNormals;
void main(){

	vec2 TexCoord = gl_FragCoord.xy / screenSize;
	//vec2 TexCoord = (clipUV.xy / clipUV.z) / 2.0 + 0.5; //just another way to get coordinates

	float depthToBottom = texture(depthMapSampler, TexCoord).r; //depth from camera to the bottom of lake
	float floorDistance = 2.0 * near * far / (far + near - (2.0 * depthToBottom - 1.0) * (far - near)); //depth converted to distance from camera to botton of lake

	float depthToFragment = gl_FragCoord.z;
	float waterDistance = 2.0 * near * far / (far + near - (2.0 * depthToFragment - 1.0) * (far - near)); //depth converted to distance from camera to the surface of water

	float waterDepth = floorDistance - waterDistance;

	vec2 tiledUV = UV * tiling;
	vec2 distortion = texture(dudvMapSampler, vec2(tiledUV.x + moveFactor, tiledUV.y)).rg * 0.1;
	distortion = tiledUV + vec2(distortion.x, distortion.y + moveFactor);
	vec2 totaldistortion = (texture(dudvMapSampler, distortion).rg * 2.0 - 1.0) * waveStrength * clamp(waterDepth / 20.0, 0.0, 1.0);

	TexCoord += totaldistortion;
	TexCoord = clamp(TexCoord, 0.001, 0.999);

	vec3 reflection = texture2D(reflectionSampler, TexCoord).rgb;
	vec3 refraction = texture2D(refractionSampler, TexCoord).rgb;

	vec3 normalMapColour = texture(normalMapSampler, distortion).rgb;
	vec3 normalFromMap = vec3(normalMapColour.r * 2.0 - 1.0, normalMapColour.b * softenNormals, normalMapColour.g * 2.0 - 1.0);
	normalFromMap = normalize(normalFromMap);

	vec3 lightDirection = normalize(LightDirection_worldspace);
	// Direction in which the triangle reflects the light
	vec3 reflectedLightDir = reflect(-lightDirection, normalFromMap);

	// Eye vector (towards the camera)
	vec3 vertexToCamera = normalize(EyeDirection_worldspace);

	float specularFactor = max(dot(reflectedLightDir, vertexToCamera), 0.0);
	float Specular = specularIntensity * pow(specularFactor, shininess);
	vec3 specularHighlights = lightColor*lightPower*Specular * clamp(waterDepth / 10.0, 0.0, 1.0);

	float fresnelFactor = dot(vertexToCamera, normalFromMap); //how much refraction replaces the reflection
	fresnelFactor = pow(fresnelFactor, fresnelAdjustment); //refraction adjustment, higher exponent == lower refraction and higher the reflection
	fresnelFactor = clamp(fresnelFactor, 0.0, 1.0); //clamp to avoid black artifacts

	refraction = mix(refraction, waterColor, clamp(waterDepth / waterRefractionDepth, 0.0, 1.0)); //blend refraction and water color based on water depth
	color.rgb = mix(reflection, refraction, fresnelFactor); //blend reflection and refraction based on fresnel factor
	color.rgb = color.rgb + specularHighlights; //apply specular
	color.a = clamp(waterDepth / maxDepthTransparent, 0.0, 1.0); //change transparency based on depth of water, more transparent near the shore where water is shallow
	float brightness = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));
	if (brightness > 1.0)
		brightColor = color;
	else
		brightColor = vec4(0.0, 0.0, 0.0, 1.0);
}
