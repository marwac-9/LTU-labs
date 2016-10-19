#version 330 core

// Ouput data
layout(location = 0) out vec2 fragmentdepthandSquaredDepth;

vec2 ComputeMoments(float Depth)  {
	vec2 Moments;  
	// First moment is the depth itself.  
	Moments.x = Depth;  
	// Compute partial derivatives of depth.  
	float dx = dFdx(Depth);  
	float dy = dFdy(Depth);  
	// Compute second moment over the pixel extents.  
	Moments.y = Depth*Depth + 0.25*(dx*dx + dy*dy);  
	return Moments;  
}

void main(){
	float depth = gl_FragCoord.z;
	fragmentdepthandSquaredDepth = ComputeMoments(depth);
}