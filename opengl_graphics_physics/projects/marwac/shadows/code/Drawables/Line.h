#pragma once
#include "MyMathLib.h"
#include <GL/glew.h>
class Mesh;
class Material;

class Line
{
	
public:
	
	Line();
	void SetUpBuffers();
	void Draw(const mwm::Matrix4& Model, const mwm::Matrix4& View, const mwm::Matrix4& Projection, float width = 4.0f);
	Mesh* mesh;
	Material* mat;
	
	static const GLushort elements[2];
	static const mwm::Vector3 vertices[2];

	GLuint MatrixHandle;
	GLuint MaterialColorValueHandle;
};
