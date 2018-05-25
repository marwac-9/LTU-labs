#ifndef mwm_findpath
#define mwm_findpath

#include "HalfEdgeMesh2DSquaredHC.h"
#include "BreadthFirst.h"
#include "FaceHC.h"

int FindPath(const int nStartX, const int nStartY, const int nTargetX, const int nTargetY, const unsigned char* pMap, const int nMapWidth, const int nMapHeight, int* pOutBuffer, const int maxPathLenght)
{
	if (pMap[nStartY*nMapWidth + nStartX] == 0 || pMap[nTargetY*nMapWidth + nTargetX] == 0)
	{
		return -1;
	}

	HalfEdgeMesh2DSquaredHC hMesh = HalfEdgeMesh2DSquaredHC();
	hMesh.Construct(pMap, nMapWidth, nMapHeight);
	FaceHC* startFace = &hMesh.faces[nStartY*nMapWidth + nStartX];
	FaceHC* goalFace = &hMesh.faces[nTargetY*nMapWidth + nTargetX];
	int mapSize = nMapWidth * nMapHeight;
	return BreadthFirst().BFS(startFace, goalFace, mapSize, pOutBuffer, maxPathLenght);
}

#endif
