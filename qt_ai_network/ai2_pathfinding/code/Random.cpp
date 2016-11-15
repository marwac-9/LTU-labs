#include "Random.h"
#include "Face.h"
#include "Edge.h"
#include "MyMathLib.h"

using namespace cop4530;

std::vector<Face*> Random::RandomSearch(Vector<Face*> &allNodes, Face* start, Face* end) {

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
						currentEdge = getRandomEdge(currentNode, currentEdge);
						break;
					}
				}
				else
				{
					currentEdge = pair;
					visitedNodes.push_back(currentEdge);
					currentEdge = getRandomEdge(currentNode, currentEdge);
					break;
				}
			}
			currentEdge = getRandomEdge(currentNode, currentEdge);
		} while (currentEdge != currentNode->edge);

		currentNode = currentEdge->face;
	}

	visitedNodes.push_back(currentEdge);
	//Build the final path
	std::vector<Face*> returnVector;
	for (int i = visitedNodes.size() - 1; i > -1; i--)
	{
		returnVector.push_back(visitedNodes.at(i)->face);
	}
	return returnVector;

}

int Random::getIndex(Face* searchNode, std::vector<Face*> &searchVector) {

	for (size_t i = 0; i < searchVector.size(); i++)
	{
		if (searchVector.at(i) == searchNode)
		{
			return i;
		}
	}
	return -1;
}

int Random::getIndex(Edge* searchNode, std::vector<Edge*> &searchVector) {

	for (size_t i = 0; i < searchVector.size(); i++)
	{
		if (searchVector.at(i) == searchNode)
		{
			return i;
		}
	}
	return -1;
}

Edge* Random::getRandomEdge(Face* face, Edge* edge)
{
	Edge* randomEdge = edge;
	while (randomEdge == edge)
	{
		int randomEdgeIndex = rand() % 5;
	
		for (int i = 0; i < randomEdgeIndex; i++)
		{
			edge = edge->next;
		}
	}
	return edge;
}