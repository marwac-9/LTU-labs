#pragma once
#include <GL/glew.h>

class Texture2D
{
public:
	Texture2D();
	~Texture2D();
	GLuint TextureID;
	GLuint TextureHandle;
	int meshID;
	void LoadTexture(char* path, const GLuint programID);
private:

};

