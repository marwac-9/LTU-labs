#include "MyMathLib.h"
class Edge;

__declspec(align(16)) class Vertex
{
public:
	mwm::Vector3 pos, normal, newPos;
	mwm::Vector2 tex;
	mwm::Vector4 color;
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

