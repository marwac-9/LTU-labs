#include "Material.h"
using namespace mwm;
Material::Material()
{
	ambient.vect[0] = 0.15f;
	ambient.vect[1] = 0.15f;
	ambient.vect[2] = 0.15f;

	specular.vect[0] = 0.3f;
	specular.vect[1] = 0.3f;
	specular.vect[2] = 0.3f;

	diffuse.vect[0] = 1.0f;
	diffuse.vect[1] = 1.0f;
	diffuse.vect[2] = 1.0f;

	color.vect[0] = 0.0f;
	color.vect[1] = 0.0f;
	color.vect[2] = 0.0f;
}

Material::~Material()
{
}

void Material::AssignTexture(Texture2D *texture)
{
	this->texture2D = texture;
}
void Material::SetDiffuseColor(float r, float g, float b)
{
	this->diffuse.vect[0] = r;
	this->diffuse.vect[1] = g;
	this->diffuse.vect[2] = b;
}

void Material::SetAmbientColor(float r, float g, float b)
{
	this->ambient.vect[0] = r;
	this->ambient.vect[1] = g;
	this->ambient.vect[2] = b;
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