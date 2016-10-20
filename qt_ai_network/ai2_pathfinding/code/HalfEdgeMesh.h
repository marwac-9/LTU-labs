#pragma once
#include <vector>
#include <list>
#include "MyMathLib.h"

class Vertex;
class Edge;
class Face;

class HalfEdgeMesh
{
public:
	HalfEdgeMesh();
	~HalfEdgeMesh();
	void Construct(const char * path);

	std::vector<Vertex*> vertices;
	std::vector<Edge*> edges;
	std::vector<Face*> faces;
	std::vector<Face*> goals;
	std::vector<mwm::Vector2> goalsPos;

	Face* startFace;
	Face* endFace;
	mwm::Vector2 startFacePos;
	mwm::Vector2 endFacePos;
	Face* findNode(mwm::Vector2 position);
	bool isPointInNode(mwm::Vector2 point, Face* node);
	void quadrangulate();
	void optimizeMesh();
	bool tryToJoin(Face*);
	bool tryToJoin2(Face*);
	void moveEdgesToFace(Edge* edgeOfAnotherFace, Face* currentFace);
	void joinSharedEdges(Face*);
	bool turnsRight(Edge*, Edge*);
	bool turnsRightOrParallel(Edge*, Edge*);
private:
	bool checkIfSameVect(mwm::Vector2 &vect1, mwm::Vector2 &vect2);
	
	
};

