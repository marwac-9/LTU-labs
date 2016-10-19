#pragma once
#include <GL/glew.h>

class Mesh
{
public:
	Mesh();
	~Mesh();
	GLuint vaoHandle;
	GLuint vertexbuffer;
	GLuint uvbuffer;
	GLuint normalbuffer;
	GLuint elementbuffer;
	int indicesSize;
private:

};

