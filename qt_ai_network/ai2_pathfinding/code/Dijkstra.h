#pragma once
#include <vector>
#include <set>

class Face;
class Edge;

class Dijkstra
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
	int getIndex(Face*, std::vector<Face*>);
	int getIndex(Edge*, std::vector<Edge*>);
public:
	Dijkstra(){}
	~Dijkstra(){}
	std::vector<Face*> DijSearch(Face*, const std::set<Face*>& goalsSet);
};