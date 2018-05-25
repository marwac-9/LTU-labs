#include "Breadth_First.h"
#include "Face.h"
#include "Edge.h"
#include <unordered_map>

BreadthFirst::BreadthFirst()
{
}

BreadthFirst::~BreadthFirst()
{
}

void BreadthFirst::BFS(Face* start, Face* goal, int mapSize, std::vector<Face*>& nodePath)
{
	Face** Queue = new Face*[mapSize];
	std::unordered_map<Face*, bool> discoveryMap;
	discoveryMap.reserve(mapSize);
	int Front = 0;
	int Back = 0;
	Queue[Back++] = start;
	discoveryMap[start] = true;
	//start->visited = true;

	Face* currentNode;

	Face* pairNode = nullptr;

	while (Front < Back)
	{
		currentNode = Queue[Front];
		Edge* currentEdge = currentNode->edge;

		do
		{
			//returns the path if we have come to our goal
			if (goal == currentNode)
			{
				if (currentNode == start)
				{
					delete[] Queue;
					return;
				}

				nodePath.push_back(goal);
				Face* currentPathNode = goal;
				Face* nextNode = nullptr;
				while ((nextNode = currentPathNode->previousFace) != start)
				{
					nodePath.push_back(nextNode);
					currentPathNode = nextNode;
				}
				delete[] Queue;
				return;
			}

			pairNode = nullptr;

			//checks if the edges are connected
			if (currentEdge->pair != nullptr) {
				pairNode = currentEdge->pair->face;
				//if pairnode has not been visited, add it to the nodes we want to visit
				//if (!pairNode->visited)
				if (!discoveryMap[pairNode]) {
					Queue[Back++] = pairNode;
					pairNode->previousFace = currentNode;
					//pairNode->visited = true;
					discoveryMap[pairNode] = true;
				}
			}
			currentEdge = currentEdge->next;

		} while (currentEdge != currentNode->edge);
		Front++;
	}
	delete[] Queue;
}