#pragma once
#include <vector>
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

