#pragma once
#include <vector>

class Face;
class Edge;

class TurnLeft
{
private:
	//All edges that has been discovered
	std::vector<Edge*> visitedNodes;
	//The edge that discovered the edge
	std::vector<Edge*> path;


	int getIndex(Face*, std::vector<Face*>&);
	int getIndex(Edge*, std::vector<Edge*>&);
public:
	TurnLeft(){}
	~TurnLeft(){}
	std::vector<Face*> TurnLeftSearch(std::vector<Face*>&, Face*, Face*);
};