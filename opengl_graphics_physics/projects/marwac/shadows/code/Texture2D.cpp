#include "Texture2D.h"
#include "GraphicsManager.h"


Texture2D::Texture2D()
{
}

Texture2D::~Texture2D()
{
}

void Texture2D::LoadTexture(char* path, const GLuint programID)
{
	// Load the texture
	this->TextureID = GraphicsManager::LoadDDS(path);

	// Get a handle for our "myTextureSampler" uniform
	this->TextureHandle = glGetUniformLocation(programID, "myTextureSampler");
}