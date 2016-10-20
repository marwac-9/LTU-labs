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

	stack.push_back(start);
	Face* currentNode;
	visitedNodes.push_back(start);
	std::map<Face*, Face*> discoveryMap;
	//Vad blev upptäckt, av vem
	while (stack.size() > 0) {

		currentNode = stack.front();
		Edge* currentEdge = currentNode->edge;

		do
		{
			Face* pairNode = NULL;

			//checks if the edges are connected
			if (currentEdge->pair != NULL) {
				if (currentEdge->pair->face != NULL)
					pairNode = currentEdge->pair->face;
			}

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
			if (pairNode != NULL)
			{
				//if pairnode has not been visited, add it to the nodes we want to visit
				if (!checkIfVisited(pairNode)) {
					visitedNodes.push_back(pairNode);
					stack.push_back(pairNode);
					discoveryMap.insert(std::pair<Face*, Face*>(pairNode, currentNode));
				}
			}
			currentEdge = currentEdge->next;

		} while (currentEdge != currentNode->edge);

		stack.erase(stack.begin());

	}
	std::vector<Face*> nodePath;
	return nodePath;

}

bool BreadthFirst::checkIfVisited(Face* pairNode) {

	for (size_t i = 0; i < visitedNodes.size(); i++)
	{
		if (visitedNodes.at(i) == pairNode)
		{
			return true;
		}
	}
	return false;
}