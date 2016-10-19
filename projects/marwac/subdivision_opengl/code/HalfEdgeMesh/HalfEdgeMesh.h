#pragma once
#include "MyMathLib.h"
#include "Mesh.h"
#include "PoolParty.h"
#include "Vector.h"

using namespace cop4530;
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
	PoolParty<Vertex,1000> vertexPool;
	PoolParty<Edge,1000> edgePool;
	PoolParty<Face,1000> facePool;
	Vector<Vertex*> vertices;
	Vector<Edge*> edges;
	Vector<Face*> faces;
	bool checkIfSameVect(mwm::Vector3 &vect1, mwm::Vector3 &vect2);
	void SplitHalfEdges();
	void CalculateOldPosition();
	void CalculateMidpointPosition();
	void UpdateVertexPositions();
	void UpdateConnections();
};

