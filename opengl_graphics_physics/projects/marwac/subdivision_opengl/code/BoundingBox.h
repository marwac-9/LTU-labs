#pragma once
#include "MyMathLib.h"
#include <GL/glew.h>
class Mesh;
class Material;

struct MinMax
{
	mwm::Vector3 min;
	mwm::Vector3 max;
};

struct OBB
{
	mwm::Vector3 halfExtent;//only obb
	mwm::Matrix3 model;
	//prob should include halfExtend rather than scale, halfExtent = 0.5*scale
	//prob should include rot matrix instead of model it should not have the scale in it
	MinMax mm;
	mwm::Vector3 color;
};

struct AABB
{
	mwm::Matrix4 model;
	mwm::Vector3 color;
};

class BoundingBox
{
	
public:
	
	BoundingBox();
	void SetUpBuffers();
	void Draw(const mwm::Matrix4& Model, const mwm::Matrix4& View, const mwm::Matrix4& Projection);
	MinMax CalcValuesInWorld(const mwm::Matrix3& modelMatrix, const mwm::Vector3& position) const;
	//Vector3 min = Vector3(-0.5, -0.5, -0.5);
	//Vector3 max = Vector3(0.5, 0.5, 0.5);
	//Vector3 center = Vector3(0, 0, 0);
	//Vector3 dim = Vector3(1, 1, 1);
	Mesh* mesh;
	Material* mat;
	
	static const GLushort elements[24];
	static const mwm::Vector3 vertices[8];

	GLuint MatrixHandle;
	GLuint MaterialColorValueHandle;
};
