#pragma once
#include "MyMathLib.h"
#include <GL/glew.h>
class Mesh;
class Material;

class Plane
{
	
public:
	
	Plane();
	void SetUpBuffers();
	void Draw(const mwm::Matrix4& Model, const mwm::Matrix4& View, const mwm::Matrix4& Projection);
	Mesh* mesh;
	Material* mat;
	
	static const GLushort elements[6];
	static const mwm::Vector3 vertices[4];

	GLuint MatrixHandle;
	GLuint MaterialColorValueHandle;
};
