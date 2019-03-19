#version 420 core

layout(location = 0) in vec2 vertexPosition_modelspace;

out vec3 Position_worldspace;
out vec3 EyeDirection_worldspace;
out vec3 LightDirection_worldspace;
out vec3 clipUV;
out float moveFactor;
out vec2 UV;

uniform mat4 MVP;
uniform mat4 M;
uniform vec3 CameraPos;
uniform vec3 LightInvDirection_worldspace;
uniform float fTime;
uniform int waterSize;
/* //WIP
float amplitude = 0.6f;
float speed = 3.f;
float periodicScale = 200.f;

float calculateHeightFor(vec2 pos){
	float height = 0;
	height += (amplitude * sin(speed*fTime + periodicScale*pos.x));
	height += (amplitude * sin(speed*fTime + periodicScale*pos.y));
	return height;
}
//END WIP */

void main(){

	// Output position of the vertex, in clip space : MVP * position
	vec3 pos_model = vec3(vertexPosition_modelspace.x, 0.0, vertexPosition_modelspace.y);
	
	/* //WIP
	pos_model.y = calculateHeightFor(pos_model.xz);

	// read neighbor heights using an arbitrary small offset
	vec3 off = vec3(1.0, 1.0, 0.0);
	float hL = calculateHeightFor(pos_model.xz - off.xz);
	float hR = calculateHeightFor(pos_model.xz + off.xz);
	float hD = calculateHeightFor(pos_model.xz - off.zy);
	float hU = calculateHeightFor(pos_model.xz + off.zy);

	vec3 Normal;
	Normal.x = hL - hR;
	Normal.y = hD - hU;
	Normal.z = 2.0;
	Normal = normalize(Normal);
	//END WIP */

	gl_Position = MVP * vec4(pos_model, 1);
	clipUV = gl_Position.xyw;
	
	Position_worldspace = (M * vec4(pos_model, 1)).xyz;
	EyeDirection_worldspace = CameraPos - Position_worldspace;
	LightDirection_worldspace = LightInvDirection_worldspace;
	UV = vertexPosition_modelspace / waterSize;

	moveFactor = fTime;
}