#include "Depth_first.h"
#include "Face.h"
#include "Edge.h"

DepthFirst::DepthFirst()
{
}

DepthFirst::~DepthFirst()
{
}

std::vector<Face*> DepthFirst::DFS(Face* start, Face* end)
{

	stack.push_back(start);
	Face* currentNode;

	while (stack.size() > 0) {
		
		currentNode = stack.back();

		visitedNodes.push_back(currentNode);

		bool found = false;

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
			if (currentNode == end) {
				std::vector<Face*> nodePath;
				for (size_t i = 0; i < stack.size(); i++)
				{
					nodePath.push_back(stack.at(i));
				}
				//returns the path we want
				std::reverse(nodePath.begin(), nodePath.end());
				return nodePath;
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
	std::vector<Face*> nodePath;
	return nodePath;

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
