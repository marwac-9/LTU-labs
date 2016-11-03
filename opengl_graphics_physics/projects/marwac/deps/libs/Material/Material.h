#pragma once
#include "MyMathLib.h"
#include "Texture2D.h"

class Material
{
public:
	Material();
	~Material();
	Texture2D* texture2D;
	mwm::Vector3 emission;    // Ecm   
	float ambientIntensity;     // Acm   
	float diffuseIntensity;     // Dcm   diffuse sets is the diffuse intensity, currently diffuse color is obtained from texture
	mwm::Vector3 color;		 // color over texture if texture not preset used as shader diffuse color
	float specularIntensity;    // Scm   
	float shininess;  // Srm  
	void AssignTexture(Texture2D *texture);
	void SetDiffuseIntensity(float d);
	void SetSpecularIntensity(float spec);
	void SetShininess(float s);
	void SetAmbientIntensity(float a);
	void SetColor(float r, float g, float b);
	void SetColor(const mwm::Vector3& colorC);
	
private:

};

