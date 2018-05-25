#include "Dijkstra.h"
#include "Face.h"
#include "Edge.h"

void Dijkstra::DijSearch(Face* start, Face* goal, std::vector<Face*>& nodePath) {

	Face* currentNode = start;
	Edge* previousEdge = NULL;

	//As long as we are not in the end node
	while (goal != currentNode)
	{
		Edge* currentEdge = currentNode->edge;
		//Looping through every edge in the current node
		do
		{
			//Check to see if the edge has a pair
			if (currentEdge->pair != NULL && !checkIfVisited(currentEdge) && !checkIfVisited(currentEdge->pair))
			{
				float cost;
				//For now we set the cost to 0 if it is the first edge
				if (previousEdge == NULL)
				{
					cost = 0;
				}
				else
				{
					//Get the index to the previous edge
					int index = getIndex(previousEdge, visitedNodes);
					//If the opposite(pair) edge has been indexed, get that instead
					if (index == -1)
					{
						index = getIndex(previousEdge->pair, visitedNodes);
					}
					//Calculate the new cost
					float previousCost = distance.at(index);
					float newCost = (previousEdge->Midpoint() - currentEdge->Midpoint()).vectLengt();
					cost = previousCost + newCost;
				}

				//Check if the edge has been visisted already
				if (checkIfUnvisited(currentEdge) || checkIfUnvisited(currentEdge->pair))
				{

					//Get the index to the current edge
					int index = getIndex(currentEdge, visitedNodes);
					//If the opposite(pair) edge has been indexed, get that instead
					if (index == -1)
					{
						index = getIndex(currentEdge->pair, visitedNodes);
					}
					//Calculate the cost for the other edge
					float oldCost = distance.at(index);
					if (cost < oldCost)
					{
						distance.at(index) = cost;
						path.at(index) = previousEdge;
					}

				}
				else
				{
					//Add the new edge to all the lists
					visitedNodes.push_back(currentEdge);
					path.push_back(previousEdge);
					distance.push_back(cost);
					unvisitedList.push_back(currentEdge);
				}
			}
			currentEdge = currentEdge->next;
		} while (currentEdge != currentNode->edge);

		if (previousEdge != NULL)
		{
			//Remove the previous edge from the unvisited list
			int removeIndex = getIndex(previousEdge, unvisitedList);
			unvisitedList.erase(unvisitedList.begin() + removeIndex);
		}

		//Get the edge with the lowest cost
		int lowestCostIndex = 0;
		float lowestCost = distance.at(getIndex(unvisitedList.at(0), visitedNodes));
		for (size_t i = 0; i < unvisitedList.size(); i++)
		{
			float costToEdge = distance.at(getIndex(unvisitedList.at(i), visitedNodes));
			if (costToEdge < lowestCost)
			{
				lowestCost = costToEdge;
				lowestCostIndex = i;
			}
		}
		//Set the previous(next) edge and the current(next) node
		previousEdge = unvisitedList.at(lowestCostIndex);
		if (previousEdge->pair->face == currentNode)
		{
			currentNode = previousEdge->face;
		}
		else
		{
			currentNode = previousEdge->pair->face;
		}
	}
	//Build the final path
	Edge* tempEdge = previousEdge;
	nodePath.push_back(currentNode);
	if (previousEdge != NULL)
	{
		nodePath.push_back(previousEdge->face);
	}
	
	while (tempEdge != NULL) {
		int index = getIndex(tempEdge, visitedNodes);
		if (index == -1)
		{
			index = getIndex(tempEdge->pair, visitedNodes);
		}

		if (path.at(index) == NULL)
		{
			break;
		}

		nodePath.push_back(path.at(index)->face);
		tempEdge = path.at(index);
	}
	nodePath.pop_back();
}

bool Dijkstra::checkIfVisited(Edge* pairNode) {
	for (size_t i = 0; i < visitedNodes.size(); i++)
	{
		if (visitedNodes.at(i) == pairNode)
		{
			return true;
		}
	}
	return false;
}

bool Dijkstra::checkIfUnvisited(Edge* pairNode) {
	for (size_t i = 0; i < visitedNodes.size(); i++)
	{
		if (visitedNodes.at(i) == pairNode)
		{
			return true;
		}
	}
	return false;
}

int Dijkstra::getIndex(Face* searchNode, std::vector<Face*> searchVector) {

	for (size_t i = 0; i < searchVector.size(); i++)
	{
		if (searchVector.at(i) == searchNode)
		{
			return i;
		}
	}
	return -1;
}

int Dijkstra::getIndex(Edge* searchNode, std::vector<Edge*> searchVector) {

	for (size_t i = 0; i < searchVector.size(); i++)
	{
		if (searchVector.at(i) == searchNode)
		{
			return i;
		}
	}
	return -1;
}