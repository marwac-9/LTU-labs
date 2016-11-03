#pragma once
#include "MyMathLib.h"

class Mesh;
class Material;

class Plane
{
	
public:
	
	Plane();
	void SetUpBuffers();
	void Draw(const mwm::Matrix4& Model, const mwm::Matrix4& View, const mwm::Matrix4& Projection, const unsigned int shader);
	Mesh* mesh;
	Material* mat;
	
	static const unsigned short elements[6];
	static const mwm::Vector3 vertices[4];

	unsigned int MatrixHandle;
	unsigned int MaterialColorValueHandle;
};
