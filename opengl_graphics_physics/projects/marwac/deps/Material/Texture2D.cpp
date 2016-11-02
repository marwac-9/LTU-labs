#include "Texture2D.h"
#include <GL/glew.h>

Texture2D::Texture2D()
{
}

Texture2D::~Texture2D()
{
	glDeleteBuffers(1, &TextureID);
}