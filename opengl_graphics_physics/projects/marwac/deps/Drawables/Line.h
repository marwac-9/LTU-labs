#pragma once
#include "MyMathLib.h"

class Mesh;
class Material;

class Line
{
	
public:
	
	Line();
	void SetUpBuffers();
	void Draw(const mwm::Matrix4& Model, const mwm::Matrix4& View, const mwm::Matrix4& Projection, const unsigned int shader, float width = 4.0f);
	Mesh* mesh;
	Material* mat;
	
	static const unsigned short elements[2];
	static const mwm::Vector3 vertices[2];

	unsigned int MatrixHandle;
	unsigned int MaterialColorValueHandle;
};
