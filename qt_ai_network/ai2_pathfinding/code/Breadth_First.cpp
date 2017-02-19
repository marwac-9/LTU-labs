#include "Breadth_First.h"
#include "Face.h"
#include "Edge.h"
#include <map>

BreadthFirst::BreadthFirst()
{
}

BreadthFirst::~BreadthFirst()
{
}

std::vector<Face*> BreadthFirst::BFS(Face* start, const std::set<Face*>& goalsSet)
{
	std::vector<Face*> queue;
	queue.push_back(start);
	Face* currentNode;
	std::map<Face*, Face*> discoveryMap;
	Face* pairNode;
	//Vad blev upptäckt, av vem
	while (queue.size() > 0) {

		currentNode = queue.front();
		Edge* currentEdge = currentNode->edge;

		do
		{
			//returns the path if we have come to our goal
			std::set<Face*>::iterator end = goalsSet.find(currentNode);
			if (goalsSet.end() != end)
			{
				std::vector<Face*> nodePath;
				nodePath.push_back(*end);
				Face* currentPathNode = *end;
				while (currentPathNode != start)
				{
					Face* nextNode = discoveryMap.at(currentPathNode);
					nodePath.push_back(nextNode);
					currentPathNode = nextNode;
				}
				return nodePath;
			}

			pairNode = NULL;

			//checks if the edges are connected
			if (currentEdge->pair != NULL) {
				pairNode = currentEdge->pair->face;
				//if pairnode has not been visited, add it to the nodes we want to visit
				if (discoveryMap.find(pairNode) == discoveryMap.end()) {
					queue.push_back(pairNode);
					discoveryMap.insert(std::pair<Face*, Face*>(pairNode, currentNode));
				}
			}

			currentEdge = currentEdge->next;

		} while (currentEdge != currentNode->edge);

		queue.erase(queue.begin());

	}
	std::vector<Face*> nodePath;
	return nodePath;

}