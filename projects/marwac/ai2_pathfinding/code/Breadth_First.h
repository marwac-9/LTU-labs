#pragma once
#include <vector>
#include <set>
class Face;
class Edge;

class BreadthFirst
{
private:
	std::vector<Face*> visitedNodes;
	std::vector<Face*> stack;
	std::vector<Face*> path;
	bool checkIfVisited(Face*);
	bool checkIfPath(Face*);
public:
	BreadthFirst();
	~BreadthFirst();
	std::vector<Face*> BFS(Face* startFace, const std::set<Face*>& goalsSet);
};