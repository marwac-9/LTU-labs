#pragma once
#include <vector>

class Face;
class Edge;

class AStar
{
private:
	//All edges that has been discovered
	std::vector<Edge*> visitedNodes;
	//The edge that discovered the edge
	std::vector<Edge*> path;
	//The distance to all edges
	std::vector<float> distance;
	//The edges we can go to
	std::vector<Edge*> unvisitedList;

	//std::vector<Node*> visitedNodes;
	//bool checkIfVisited(Node*);
	bool checkIfVisited(Edge*);
	bool checkIfUnvisited(Edge*);
	int getIndex(Face*, const std::vector<Face*>&);
	int getIndex(Edge*, const std::vector<Edge*>&);
public:
	AStar(){}
	~AStar(){}
	void AStarSearch(Face*, Face*, std::vector<Face*>& nodePath);
};