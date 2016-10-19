#include "TurnLeft.h"
#include "Face.h"
#include "Edge.h"
#include "MyMathLib.h"

std::vector<Face*> TurnLeft::TurnLeftSearch(std::vector<Face*> &allNodes, Face* start, Face* end) {

	visitedNodes = std::vector<Edge*>();
	Face* currentNode = start;
	Edge* currentEdge = currentNode->edge;
	visitedNodes.push_back(currentEdge);
	//As long as we are not in the end node
	while (currentNode != end)
	{
		//Looping through every edge in the current node
		do
		{
			Edge* pair = currentEdge->pair;
			//try to get to the next node
			if (pair)
			{
				if (pair->next->pair)
				{
					if (pair->next->pair->face != currentNode)
					{
						currentEdge = pair;
						visitedNodes.push_back(currentEdge);
						currentEdge = currentEdge->next;
						break;
					}
				}
				else
				{
					currentEdge = pair;
					visitedNodes.push_back(currentEdge);
					currentEdge = currentEdge->next;
					break;
				}
			}
			currentEdge = currentEdge->next;
		} while (currentEdge != currentNode->edge);

		currentNode = currentEdge->face;
	}

	visitedNodes.push_back(currentEdge);
	//Build the final path
	std::vector<Face*> returnVector;
	for (int i = visitedNodes.size()-1; i > -1; i--)
	{
		returnVector.push_back(visitedNodes.at(i)->face);
	}
	return returnVector;

}

int TurnLeft::getIndex(Face* searchNode, std::vector<Face*> &searchVector) {

	for (size_t i = 0; i < searchVector.size(); i++)
	{
		if (searchVector.at(i) == searchNode)
		{
			return i;
		}
	}
	return -1;
}

int TurnLeft::getIndex(Edge* searchNode, std::vector<Edge*> &searchVector) {

	for (size_t i = 0; i < searchVector.size(); i++)
	{
		if (searchVector.at(i) == searchNode)
		{
			return i;
		}
	}
	return -1;
}