#ifndef mwm_breadthfirst
#define mwm_breadthfirst

class FaceHC;
class EdgeHC;

class BreadthFirst
{
private:
public:
	BreadthFirst();
	~BreadthFirst();
	int BFS(FaceHC* startFace, FaceHC* goal, int mapSize, int* pOutBuffer, int maxPathLenght);
};

#endif
