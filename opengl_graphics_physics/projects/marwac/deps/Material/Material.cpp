#include "Material.h"

using namespace mwm;

Material::Material()
{
	ambientIntensity = 0.15f;

	specular.vect[0] = 0.3f;
	specular.vect[1] = 0.3f;
	specular.vect[2] = 0.3f;

	diffuseIntensity = 1.f;

	color.vect[0] = 0.f;
	color.vect[1] = 0.f;
	color.vect[2] = 0.f;

	shininess = 10.0f;
}

Material::~Material()
{
}

void Material::AssignTexture(Texture2D *texture)
{
	this->texture2D = texture;
}
void Material::SetDiffuseIntensity(float d)
{
	this->diffuseIntensity = d;
}

void Material::SetAmbientIntensity(float a)
{
	this->ambientIntensity = a;
}

void Material::SetSpecularColor(float r, float g, float b)
{
	this->specular.vect[0] = r;
	this->specular.vect[1] = g;
	this->specular.vect[2] = b;
}

void Material::SetColor(float r, float g, float b)
{
	this->color.vect[0] = r;
	this->color.vect[1] = g;
	this->color.vect[2] = b;
}

void Material::SetShininess(float s)
{
	this->shininess = s;
}