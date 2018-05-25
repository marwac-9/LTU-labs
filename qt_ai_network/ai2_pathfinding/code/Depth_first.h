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
	void DFS(Face*, Face*, std::vector<Face*>& nodePath);
};