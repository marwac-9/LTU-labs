#pragma once
#include <vector>
#include "MyMathLib.h"
#include "Mesh.h"
class Vertex;
class Edge;
class Face;
class OBJ;

class HalfEdgeMesh: public Mesh
{
public:
	HalfEdgeMesh();
	~HalfEdgeMesh();
	void Construct(OBJ &object);
	void Subdivide();
	static void ExportMeshToOBJ(HalfEdgeMesh* mesh, OBJ* newOBJ);
	

private:
	std::vector<Vertex*> vertices;
	std::vector<Edge*> edges;
	std::vector<Face*> faces;
	bool checkIfSameVect(mwm::Vector3 &vect1, mwm::Vector3 &vect2);
	void SplitHalfEdges();
	void CalculateOldPosition();
	void CalculateMidpointPosition();
	void UpdateVertexPositions();
	void UpdateConnections();
};

