#include "Depth_first.h"
#include "Face.h"
#include "Edge.h"

DepthFirst::DepthFirst()
{
}

DepthFirst::~DepthFirst()
{
}

void DepthFirst::DFS(Face* start, Face* end, std::vector<Face*>& nodePath)
{
	std::vector<Face*> stack;
	stack.push_back(start);
	Face* currentNode;
	Face* pairNode;
	while (stack.size() > 0) {
		
		currentNode = stack.back();

		visitedNodes.push_back(currentNode);

		bool found = false;

		Edge* currentEdge = currentNode->edge;

		do
		{
			//returns the path if we have come to our goal
			if (currentNode == end) {
				for (int i = stack.size() - 1; i > 0; i--)
				{
					nodePath.push_back(stack.at(i));
				}
				//returns the path we want
				return;
			}

			pairNode = NULL;

			//checks if the edges are connected
			if (currentEdge->pair != NULL) {
				pairNode = currentEdge->pair->face;
			}

			if (pairNode != NULL)
			{
				//if pairnode has not been visited, add it to the nodes we want to visit
				if (!checkIfVisited(pairNode)) {
					stack.push_back(pairNode);
					found = true;
					break;
				}
			}

			currentEdge = currentEdge->next;

		} while (currentEdge != currentNode->edge);

		if (!found) {
			stack.pop_back();
		}

	}
}

bool DepthFirst::checkIfVisited(Face* pairNode) {

	for (size_t i = 0; i < visitedNodes.size(); i++)
	{
		if (visitedNodes.at(i) == pairNode)
		{
			return true;
		}
	}
	return false;
}
