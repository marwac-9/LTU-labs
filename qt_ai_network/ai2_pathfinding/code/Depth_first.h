#pragma once
#include <vector>
class Face;
class Edge;

class DepthFirst
{
private:
	std::vector<Face*> visitedNodes;
	bool checkIfVisited(Face*);
	bool checkIfPath(Face*);
public:
	DepthFirst();
	~DepthFirst();
	std::vector<Face*> DFS(Face*, Face*);
};