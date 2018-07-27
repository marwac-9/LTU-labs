#include "MyMathLib.h"
class Edge;

class Vertex
{
public:
	mwm::Vector3F pos, normal, newPos;
	mwm::Vector2F tex;
	mwm::Vector4F color;
	Edge * edge;
	Vertex();
	~Vertex();
private:

};

