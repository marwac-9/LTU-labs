#pragma once
#include <GL/glew.h>
#include <vector>
#include "MyMathLib.h"
#include "Node.h"
class Node;
class Material;
class Mesh;

class Object
{
public:
	Object();
	~Object();
	Node node;
	Material* mat;
	Mesh* mesh;
	int ID;
	void AssignMesh(Mesh* mesh);
	int indicesSize;
private:

};

