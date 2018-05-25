#include "BreadthFirst.h"
#include "FaceHC.h"
#include "EdgeHC.h"
#include <unordered_map>

BreadthFirst::BreadthFirst()
{
}

BreadthFirst::~BreadthFirst()
{
}

int BreadthFirst::BFS(FaceHC* start, FaceHC* goal, int mapSize, int* pOutBuffer, int maxPathLenght)
{
	FaceHC** Queue = new FaceHC*[mapSize];
	std::unordered_map<FaceHC*, bool> discoveryMap;
	int Front = 0;
	int Back = 0;
	Queue[Back++] = goal;
	//goal->visited = true; //without it we get heap corruption, it happens only on a map filled with 1 and this case is not tested, or when goal is at the end
	discoveryMap[goal] = true;
	FaceHC* currentNode = nullptr;

	bool pathFound = false;
	while (Back > Front) {

		currentNode = Queue[Front++];
		if (start == currentNode)
		{
			pathFound = true;
			break;
		}

		if (currentNode->left.pair != nullptr) {
			FaceHC* pairNode = currentNode->left.pair->face;
			if (!discoveryMap[pairNode]) {
				Queue[Back++] = pairNode;
				pairNode->previousFace = currentNode;
				discoveryMap[pairNode] = true;
			}
		}

		if (currentNode->top.pair != nullptr) {
			FaceHC* pairNode = currentNode->top.pair->face;
			if (!discoveryMap[pairNode]) {
				Queue[Back++] = pairNode;
				pairNode->previousFace = currentNode;
				discoveryMap[pairNode] = true;
			}
		}

		if (currentNode->right.pair != nullptr) {
			FaceHC* pairNode = currentNode->right.pair->face;
			if (!discoveryMap[pairNode]) {
				Queue[Back++] = pairNode;
				pairNode->previousFace = currentNode;
				discoveryMap[pairNode] = true;
			}
		}

		if (currentNode->bottom.pair != nullptr) {
			FaceHC* pairNode = currentNode->bottom.pair->face;
			if (!discoveryMap[pairNode]) {
				Queue[Back++] = pairNode;
				pairNode->previousFace = currentNode;
				discoveryMap[pairNode] = true;
			}
		}
	}
	int pathLength = -1;
	if (pathFound)
	{
		pathLength = 0;
		while (currentNode != goal)
		{
			currentNode = currentNode->previousFace;
			if (pathLength < maxPathLenght)
			{
				pOutBuffer[pathLength] = currentNode->id;
			}
			pathLength++;
		}
	}

	delete[] Queue;
	return pathLength;

}