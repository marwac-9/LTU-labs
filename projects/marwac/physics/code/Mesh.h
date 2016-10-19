#pragma once
#include <GL/glew.h>
#include <vector>
#include "MyMathLib.h"
class OBJ;

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
	OBJ* obj;
private:

};

