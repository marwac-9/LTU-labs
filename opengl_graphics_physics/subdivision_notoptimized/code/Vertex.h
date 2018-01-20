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
	void* operator new(size_t i)
	{
		return _mm_malloc(i, 16);
	}

		void operator delete(void* p)
	{
		_mm_free(p);
	}
private:

};

