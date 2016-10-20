#pragma once
#include "MyMathLib.h"
#include <GL/glew.h>
class Mesh;
class Material;

class Point
{
	
public:
	
	Point();
	void SetUpBuffers();
	void Draw(const mwm::Matrix4& Model, const mwm::Matrix4& View, const mwm::Matrix4& Projection, float size = 10.0f);
	Mesh* mesh;
	Material* mat;
	
	static const GLushort elements[1];
	static const mwm::Vector3 vertices[1];

	GLuint MatrixHandle;
	GLuint MaterialColorValueHandle;
};
