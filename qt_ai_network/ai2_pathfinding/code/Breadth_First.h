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
	void BFS(Face* startFace, Face* goal, int mapSize, std::vector<Face*>& nodePath);
};