#pragma once
#include <vector>
#include <set>
class Face;
class Edge;

class BreadthFirst
{
private:

public:
	BreadthFirst();
	~BreadthFirst();
	std::vector<Face*> BFS(Face* startFace, const std::set<Face*>& goalsSet);
};