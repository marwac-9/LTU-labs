#pragma once
#include <vector>
#include "Vector.h"

class Face;
class Edge;

class Random
{
private:
	//All edges that has been discovered
	std::vector<Edge*> visitedNodes;
	//The edge that discovered the edge
	std::vector<Edge*> path;

	Edge* getRandomEdge(Face* face, Edge* edge);
	int getIndex(Face*, std::vector<Face*>&);
	int getIndex(Edge*, std::vector<Edge*>&);
public:
	Random(){}
	~Random(){}
	void RandomSearch(cop4530::Vector<Face*>&, Face*, Face*, std::vector<Face*>& nodePath);
};