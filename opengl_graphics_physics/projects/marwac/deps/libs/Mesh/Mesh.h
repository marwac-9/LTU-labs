#pragma once
class OBJ;

class Mesh
{
public:
	Mesh();
	~Mesh();
	unsigned int vaoHandle;
	unsigned int vertexbuffer;
	unsigned int uvbuffer;
	unsigned int normalbuffer;
	unsigned int elementbuffer;
	int indicesSize;
	OBJ* obj;
private:

};

