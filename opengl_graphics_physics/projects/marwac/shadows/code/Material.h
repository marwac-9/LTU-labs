#pragma once
#include <GL/glew.h>
#include "Texture2D.h"
#include "MyMathLib.h"

class Material
{
public:
	Material();
	~Material();
	Texture2D* texture2D;
	mwm::Vector3 emission;    // Ecm   
	mwm::Vector3 ambient;     // Acm   
	mwm::Vector3 diffuse;     // Dcm   diffuse sets is the diffuse intensity, currently diffuse color is obtained from texture
	mwm::Vector3 color;		 // color over texture if texture not preset used as shader diffuse color
	mwm::Vector3 specular;    // Scm   
	float shininess;  // Srm  
	void AssignTexture(Texture2D *texture);
	void SetDiffuseColor(float r, float g, float b);
	void SetAmbientColor(float r, float g, float b);
	void SetSpecularColor(float r, float g, float b);
	void SetColor(float r, float g, float b);

	void SetDiffuseColor(const mwm::Vector3& diffColor) { diffuse = diffColor; }
	void SetAmbientColor(const mwm::Vector3& ambColor) { ambient = ambColor; }
	void SetSpecularColor(const mwm::Vector3& specColor) { specular = specColor; }
	void SetColor(const mwm::Vector3& colorC) { color = colorC; }
private:

};

